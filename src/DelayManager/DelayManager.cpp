#include "DelayManager.h"
#include <Arduino.h>

DelayManager::DelayManager() { }

void DelayManager::begin()
{
  lastTime = (unsigned int)(millis() % 65536);
}

void DelayManager::sleepReamingOf(unsigned int timeOfExecute)
{
  unsigned int deltatime = (unsigned int)(millis() % 65536) - lastTime;

  if (deltatime < timeOfExecute)
  {
    delay(timeOfExecute-deltatime);
  }
  else
  {
    //FIXME probably something go really wrong with time consume so manage this
    Serial.print(" ERROR ");
    Serial.print(deltatime);
    Serial.print(" ERROR ");
  }

  lastTime = (unsigned int)(millis() % 65536);
}