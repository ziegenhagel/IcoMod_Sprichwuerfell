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
  _progressBar = config["progressBar"];
  Serial.println("progressBar Style:");
  Serial.println(_progressBar);

  _currentIndex = 0;
  _fontSize = config["fontSize"];
}

void IcoMod_Sprichwuerfel::draw(Adafruit_ST7735* tft, unsigned int colors[], JsonArray &data, unsigned int currentIndex, unsigned int fontSize)
{

  // zeige aktuelle posts
  tft->fillScreen(colors[0]);

  if(String(_progressBar) == "top") {
      _tft->fillRect(0, 0, _tft->width(), progressHeight, colors[2]);
  } else if (String(_progressBar) == "bottom") {
      _tft->fillRect(0, _tft->height() - progressHeight, _tft->width(), progressHeight, colors[2]);
  }

  // Print random funny text, data is an array of strings
  String currentFunny = data[currentIndex];
  // TextUtils::printLinesCentered(tft, currentFunny, 20, 2, tft->height() / 6 * 5, 1, colors[1]);
  TextUtils::printLinesCentered(tft, currentFunny, 20, 2, tft->height() / 6 * 1, fontSize, colors[1]);
}

void IcoMod_Sprichwuerfel::onClick()
{
  next();
 
  _nextRefresh = millis() - 1;
}

void IcoMod_Sprichwuerfel::initialize()
{
  _nextRefresh = millis() - 1;
  _lastFetch = millis() - 1;
}

void IcoMod_Sprichwuerfel::next() {

  JsonArray data = _jsonBuffer.as<JsonArray>();

  _currentIndex += 1;
  if(_currentIndex >= data.size()) {
    _currentIndex = 0;
  }

  // if 40 or more long, call next again
  String currentFunny = data[_currentIndex];
  if(currentFunny.length() >= 40 && _fontSize == 2) {
    Serial.println("Long text, calling next again");
    next();
  }

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

  if (millis() >= _nextRefresh)
  {

    _nextRefresh = millis() + _refreshTime;

    next();

    JsonArray data = _jsonBuffer.as<JsonArray>();

    draw(_tft, _colors, data, _currentIndex, _fontSize);
  }

  // progress button on bottom 

  double progress = ( _nextRefresh - millis() ) / (double)_refreshTime ;
  int progressWidth = ( _tft->width() - (progress * _tft->width()));

  int startX = 0;

  if(String(_progressBar) == "top") {
      _tft->fillRect(startX, 0, progressWidth, progressHeight, _colors[0]);
  } else if (String(_progressBar) == "bottom") {
      _tft->fillRect(startX, _tft->height() - progressHeight, progressWidth, progressHeight, _colors[0]);
  }

}