/*
  Watchdog.h - Library to guard the temperatures
*/

#ifndef Watchdog_h
#define Watchdog_h

#include <Arduino.h>
#include <avr/wdt.h>
#include "../../src/Backstore/Backstore.h"

class Watchdog
{
  public:
    Watchdog();
    void Watchdog::begin(Backstore* _store, byte _tempCount, float *_TempSensors[], byte _humCount, float *_HumSensors[]);
    void Watchdog::refresh(unsigned int deltatime);
  private:
    unsigned int timer = 0;

    Backstore *store;

    byte tempCount = 0;
    float **TempSensors;
    float *TempMax;
    float *TempMin;
    byte humCount = 0;
    float **HumSensors;
    float *HumMax;
    float *HumMin;

    void *Watchdog::allocation(byte size, byte length);
};

#endif