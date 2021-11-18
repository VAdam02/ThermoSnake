/*
  DelayManager.h - Library to manage timing of cycles
*/

#ifndef DelayManager_h
#define DelayManager_h

#include <Arduino.h>

class DelayManager
{
  public:
    DelayManager();
    void DelayManager::begin();
    void DelayManager::sleepReamingOf(unsigned int timeOfExecute);
    unsigned int DelayManager::getDeltaTime();
  private:
    unsigned int lastTime = 0;
    unsigned int delta = 0;
};

#endif