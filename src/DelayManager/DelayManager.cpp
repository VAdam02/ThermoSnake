#include "DelayManager.h"
#include <Arduino.h>

DelayManager::DelayManager() { }

unsigned int Delay_lastTime = 0;
void DelayManager::begin()
{
  Delay_lastTime = (unsigned int)(millis() % 65536);
}

void DelayManager::sleepReamingOf(int timeOfExecute)
{
  unsigned int deltatime = (unsigned int)(millis() % 65536) - Delay_lastTime;

  if (deltatime < timeOfExecute)
  {
    delay(timeOfExecute-deltatime);
  }
  else
  {
    //FIXME probably something go really wrong with time consume so manage this
  }

  Delay_lastTime += timeOfExecute;
}