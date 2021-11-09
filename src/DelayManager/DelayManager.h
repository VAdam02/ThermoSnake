/*
  DelayManager.h - Library to manage timing of cycles
*/

#ifndef DelayManager_h
#define DelayManager_h

#include "Arduino.h"

class DelayManager
{
  public:
    DelayManager();
    void DelayManager::begin();
    void DelayManager::sleepReamingOf(unsigned int timeOfExecute);
  private:
    unsigned int lastTime;
};

#endif