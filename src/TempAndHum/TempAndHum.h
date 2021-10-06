/*
  TempAndHum.h - Library for interacting with DHT11
*/

#ifndef TempAndHum_h
#define TempAndHum_h

#include "Arduino.h"

class TempAndHum
{
  public:
    TempAndHum();
    void TempAndHum::begin();
    void TempAndHum::refresh(long time);
    float TempAndHum::getCurrentTemperature();
    float TempAndHum::getTemperature(float* range);
    float TempAndHum::getTemperature();
    float TempAndHum::getCurrentHumidity();
    float TempAndHum::getHumidity(float* range);
    float TempAndHum::getHumidity();
  private:
    int lastTime;
    float temp[];
    float hum[];
};

#endif
