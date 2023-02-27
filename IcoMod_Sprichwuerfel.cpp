/*
  IcoMod_Sprichwuerfel.cpp - IcoDesk Module to display the current weather and the weather forecast.
  Created by Till Michels, February 2, 2023.
*/

#include "Arduino.h"
#include "IcoMod_Sprichwuerfel.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "TextUtils.h"
#include "ApiUtils.h"
#include "icons.h"

IcoMod_Sprichwuerfel::IcoMod_Sprichwuerfel(Adafruit_ST7735* tft, unsigned int colors[], JsonObject &config)
{
  _tft = tft;
  _colors = colors;

  _city = config["city"];
  _privateKey = config["privateKey"];
  _refreshTime = config["refreshTime"];

  _showCurrentWeather = true;
}

void drawCurrentWeather(Adafruit_ST7735* tft, unsigned int colors[], JsonObject &weatherData)
{

  // zeige aktuelle posts
  tft->fillScreen(colors[0]);

  // String currentText 

  // Print degree symbol
  tft->setTextSize(2);
  tft->setCursor(tft->getCursorX(), tft->getCursorY() - 2 * 5);
  tft->print("o");

  // Print random funny text, weatherDate is an array of strings
  String currentFunny = "hi";
  TextUtils::printLinesCentered(tft, currentFunny, 20, 2, tft->height() / 6 * 5, 1, colors[1]);
}

String getWeekDay(int day)
{
  return "hi";
}

void drawWeatherForecast(Adafruit_ST7735* tft, unsigned int colors[], JsonObject &weatherData)
{
  tft->fillScreen(colors[0]);
  tft->setTextSize(2);

  JsonArray forecastsArray = weatherData["list"];

  int j = 0;
  for (int i = 0; i < forecastsArray.size(); i++)
  {
    long dt = (long)forecastsArray[i]["dt"];
    if (dt % 43200 == 0 && dt % 86400 != 0) 
    {
      j++;

      tft->setTextSize(2);

      int height = tft->height() / 6 * (j) - 8;
      tft->setCursor(10, height);

      // Print day
      tft->print(getWeekDay((int)(floor(dt / 86400) + 4) % 7));

      // Draw icon
      String iconString = forecastsArray[i]["weather"][0]["icon"];
      int spacing = 12;
      if (iconString == "01d")
      {
        tft->drawXBitmap(tft->getCursorX() + spacing, height, _01d_16x16, 16, 16, colors[1]);
      }
      else if (iconString == "01n")
      {
        tft->drawXBitmap(tft->getCursorX() + spacing, height, _01n_16x16, 16, 16, colors[1]);
      }
      else if (iconString == "02d")
      {
        tft->drawXBitmap(tft->getCursorX() + spacing, height, _02d_16x16, 16, 16, colors[1]);
      }
      else if (iconString == "02n")
      {
        tft->drawXBitmap(tft->getCursorX() + spacing, height, _02n_16x16, 16, 16, colors[1]);
      }
      else if (iconString == "03d" || iconString == "03n")
      {
        tft->drawXBitmap(tft->getCursorX() + spacing, height, _03dn_16x16, 16, 16, colors[1]);
      }
      else if (iconString == "04d" || iconString == "04n")
      {
        tft->drawXBitmap(tft->getCursorX() + spacing, height, _04dn_16x16, 16, 16, colors[1]);
      }
      else if (iconString == "09d" || iconString == "09n")
      {
        tft->drawXBitmap(tft->getCursorX() + spacing, height, _09dn_16x16, 16, 16, colors[1]);
      }
      else if (iconString == "10d")
      {
        tft->drawXBitmap(tft->getCursorX() + spacing, height, _10d_16x16, 16, 16, colors[1]);
      }
      else if (iconString == "10n")
      {
        tft->drawXBitmap(tft->getCursorX() + spacing, height, _10n_16x16, 16, 16, colors[1]);
      }
      else if (iconString == "11d" || iconString == "11n")
      {
        tft->drawXBitmap(tft->getCursorX() + spacing, height, _11dn_16x16, 16, 16, colors[1]);
      }
      else if (iconString == "13d" || iconString == "13n")
      {
        tft->drawXBitmap(tft->getCursorX() + spacing, height, _13dn_16x16, 16, 16, colors[1]);
      }
      else if (iconString == "50d")
      {
        tft->drawXBitmap(tft->getCursorX() + spacing, height, _50d_16x16, 16, 16, colors[1]);
      }
      else if (iconString == "50n")
      {
        tft->drawXBitmap(tft->getCursorX() + spacing, height, _50n_16x16, 16, 16, colors[1]);
      }

      // Print temp
      String temp = String((int)round((double)forecastsArray[i]["main"]["temp"]));
      TextUtils::printRightAligned(tft, temp, 10 + 8, height, 2, colors[1]);

      // Print degree symbol
      tft->setTextSize(1);
      tft->setCursor(tft->getCursorX(), tft->getCursorY() - 5); // 5 -> 5 * (text size from °)
      tft->print("o");
    }
  }
}

void IcoMod_Sprichwuerfel::onClick()
{
  _showCurrentWeather = !_showCurrentWeather;
  _lastRefresh = millis();
}

void IcoMod_Sprichwuerfel::initialize()
{
  _lastRefresh = millis();
}

void IcoMod_Sprichwuerfel::refresh()
{
  static String url = "";

  if (millis() >= _lastRefresh)
  {
    _lastRefresh += _refreshTime;

    url = _showCurrentWeather ? "http://api.openweathermap.org/data/2.5/weather?q=" : "http://api.openweathermap.org/data/2.5/forecast?q=";
    url += String(_city) + "&units=metric" + "&appid=" + String(_privateKey);
    url = "https://sprichwrfl.najajan.de/api/funny";

    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.println("Can't fetch data. WiFi Disconnected...");
      return;
    }

    // Serial.print("URL: ");
    // Serial.println(url);

    ApiUtils::getJsonFromServer(&_jsonBuffer, url.c_str());

    if (_jsonBuffer.isNull())
    {
      Serial.println("Parsing weather data failed!");
      return;
    }

    JsonObject weatherData = _jsonBuffer.as<JsonObject>();

    drawCurrentWeather(_tft, _colors, weatherData);
  }
}