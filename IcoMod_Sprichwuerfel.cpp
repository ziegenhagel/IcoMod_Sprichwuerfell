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

  _refreshTime = config["refreshTime"];
  _currentIndex = 0;
}

void draw(Adafruit_ST7735* tft, unsigned int colors[], JsonArray &data, unsigned int currentIndex)
{

  // zeige aktuelle posts
  tft->fillScreen(colors[0]);

  // Print random funny text, data is an array of strings
  String currentFunny = data[currentIndex];
  // TextUtils::printLinesCentered(tft, currentFunny, 20, 2, tft->height() / 6 * 5, 1, colors[1]);
  TextUtils::printLinesCentered(tft, currentFunny, 20, 2, tft->height() / 6 * 1, 2, colors[1]);
}

void IcoMod_Sprichwuerfel::onClick()
{
  JsonArray data = _jsonBuffer.as<JsonArray>();
  _currentIndex += (_currentIndex + 1) % data.size();
  _lastRefresh = millis();
}

void IcoMod_Sprichwuerfel::initialize()
{
  _lastRefresh = millis() - 1;
  _lastFetch = millis() - 1;
}

void IcoMod_Sprichwuerfel::refresh()
{

  if (millis() >= _lastFetch)
  {
    _lastFetch = millis() + 120 * 1000;

    String url = "http://sprichwrfel.najajan.de/api/funny";

    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.println("Can't fetch data. WiFi Disconnected...");
      return;
    }

    Serial.print("URL: ");
    Serial.println(url);

    ApiUtils::getJsonFromServer(&_jsonBuffer, url.c_str());

    if (_jsonBuffer.isNull())
    {
      Serial.println("Parsing sprichwuerfel data failed!");
      return;
    }

    JsonArray data = _jsonBuffer.as<JsonArray>();
  }

  if (millis() >= _lastRefresh)
  {

    _lastRefresh += millis() + _refreshTime;

    Serial.print("Changing Sprichwuerfel to index: ");
    Serial.println(_currentIndex);

    JsonArray data = _jsonBuffer.as<JsonArray>();
    draw(_tft, _colors, data, _currentIndex);
    _currentIndex += (_currentIndex + 1) % data.size();
  }
}