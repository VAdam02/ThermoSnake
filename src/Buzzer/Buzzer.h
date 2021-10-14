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
    void Buzzer::begin(byte pin);
    void Buzzer::refresh();
    void Buzzer::alarm(unsigned int hz, unsigned int sound, unsigned int silent, unsigned int time);
    void Buzzer::mute();
    void Buzzer::sayError();
  private:
    byte _pin;
    bool taskActive;
    bool taskSound;
    unsigned int task[];
};

#endif