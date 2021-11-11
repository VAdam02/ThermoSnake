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
    unsigned int lastTime = 0;
    byte pin;
    bool taskActive;
    bool taskSound;
    unsigned int task[4] = {100, 1, 1, 1};
};

#endif