/*
  TempAndHum.h - Library for interacting with DHT11
*/

#ifndef TempAndHum_h
#define TempAndHum_h

#include "Arduino.h"

#define LENGTH 15

class TempAndHum
{
  public:
    TempAndHum();
    void TempAndHum::begin(byte pin);
    void TempAndHum::refresh();
    float TempAndHum::getCurrentTemperature();
    float TempAndHum::getCurrentHumidity();

    float temperature = NAN;
    float temperatureRange = NAN;
    float humidity = NAN;
    float humidityRange = NAN;
  private:
    float TempAndHum::getTemperature();
    float TempAndHum::getTemperature(float* range);
    float TempAndHum::getHumidity();
    float TempAndHum::getHumidity(float* range);
    
    float DHTtemp[LENGTH];
    float DHThum[LENGTH];

    unsigned int lastTime = 0;
    byte pin;

    void TempAndHum::init();
    bool TempAndHum::read();
    uint32_t TempAndHum::expectPulse(bool level);

    #ifdef __AVR
      // Use direct GPIO access on an 8-bit AVR so keep track of the port and
      // bitmask for the digital pin connected to the DHT.  Other platforms will use
      // digitalRead.
      uint8_t _bit, _port;
    #endif
    uint32_t _maxcycles;
};

#endif
