/*
  PT100.h - Library for interacting with DHT11
*/

#ifndef PT100_h
#define PT100_h

#include "Arduino.h"

class PT100
{
  public:
    PT100();
    void PT100::begin();
    void PT100::refresh();
    float PT100::getCurrentTemperature();

    float temperature = 0;
    float temperatureRange = 0;
  private:
    float PT100::getTemperature();
    float PT100::getTemperature(float* range);

    float PT100::convertValueToCelsius(unsigned int val1, unsigned int val2);
    
    unsigned int lastTime = 0;
};

#endif