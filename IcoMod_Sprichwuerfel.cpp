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
  _fontSize = config["fontSize"];
  _progressBar = config["progressBar"];
  _currentIndex = 0;
}

void draw(Adafruit_ST7735* tft, unsigned int colors[], JsonArray &data, unsigned int currentIndex, unsigned int fontSize)
{

  // zeige aktuelle posts
  tft->fillScreen(colors[0]);

  // Print random funny text, data is an array of strings
  String currentFunny = data[currentIndex];
  // TextUtils::printLinesCentered(tft, currentFunny, 20, 2, tft->height() / 6 * 5, 1, colors[1]);
  TextUtils::printLinesCentered(tft, currentFunny, 20, 2, tft->height() / 6 * 1, fontSize, colors[1]);
}

void IcoMod_Sprichwuerfel::onClick()
{
  JsonArray data = _jsonBuffer.as<JsonArray>();

  _currentIndex += 1;
  if(_currentIndex >= data.size()) {
    _currentIndex = 0;
  }

  Serial.print("Click at ");
  Serial.print(millis()); 

  _lastRefresh = millis() - 1;
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
    _lastFetch = millis() + 30 * 1000;

    String url =  "http://sprichwrfel.najajan.de/api/funny";

    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.println("Can't fetch me.");
      return;
    }

    Serial.print("URL at ");
    Serial.print(millis());

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

    _lastRefresh = millis() + _refreshTime;

    JsonArray data = _jsonBuffer.as<JsonArray>();

    _currentIndex += 1;
    if(_currentIndex >= data.size()) {
      _currentIndex = 0;
    }

    draw(_tft, _colors, data, _currentIndex, _fontSize);
  }

  // progress button on bottom 
  int progress = (_lastRefresh - millis()) / _refreshTime ;

  Serial.print("Progress: ");
  Serial.println(progress);
  Serial.print("%");

  int progressWidth = progress / _tft->width();

  // progressWidth = 40;

  // _tft->fillRect(0, 0, progressWidth, 5, _colors[2]);
  _tft->fillRect(0, _tft->height() - 5, progressWidth, 5, _colors[2]);

}