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
    void TempAndHum::refresh();
    float TempAndHum::getCurrentTemperature();
    float TempAndHum::getCurrentHumidity();

    float temperature = 0;
    float temperatureRange = 0;
    float humidity = 0;
    float humidityRange = 0;
  private:
    float TempAndHum::getTemperature();
    float TempAndHum::getTemperature(float* range);
    float TempAndHum::getHumidity();
    float TempAndHum::getHumidity(float* range);
    
    unsigned int lastTime = 0;
};

#endif