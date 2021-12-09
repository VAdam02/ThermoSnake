/*
  Watchdog.h - Library to guard the temperatures
*/

#ifndef Watchdog_h
#define Watchdog_h

#include <Arduino.h>
#include <avr/wdt.h>

class Watchdog
{
  public:
    Watchdog();
    void Watchdog::begin();
    void Watchdog::refresh(unsigned int deltatime);
  private:
    unsigned int timer = 0;
};

#endif