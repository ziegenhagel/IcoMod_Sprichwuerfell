/*
  IcoMod_Sprichwuerfel.cpp - IcoDesk Module to display a random funny mashup of two sayings.
  Created by Dominik Schürmann, February 2, 2023.
*/

#ifndef IcoMod_Sprichwuerfel_h
#define IcoMod_Sprichwuerfel_h

#include "Arduino.h"
#include "IcoMod.h"

class IcoMod_Sprichwuerfel : public IcoMod
{
  public:
    IcoMod_Sprichwuerfel(Adafruit_ST7735* tft, unsigned int colors[], JsonObject &config);
    void onClick();
    void initialize();
    void refresh();
  private:
    int _currentIndex;
    StaticJsonDocument<24576> _jsonBuffer;
    unsigned long _lastRefresh;
    unsigned long _lastFetch;
    unsigned long _fontSize;
    unsigned long _refreshTime;
};

#endif