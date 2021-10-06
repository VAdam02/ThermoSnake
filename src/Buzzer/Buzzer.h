/*
  Buzzer.h - Library for interacting with buzzer
*/

#ifndef Buzzer_h
#define Buzzer_h

#include "Arduino.h"

class Buzzer
{
  public:
    Buzzer();
    void begin(byte pin);
    void refresh(long time);
    void alarm(int hz, int sound, int silent, int time);
    void mute();
    void sayError();
  private:
    int _pin;
    bool taskActive;
    bool taskSound;
    int task[];
};

#endif