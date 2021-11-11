#include <Arduino.h>
#include "DelayManager.h"

DelayManager::DelayManager() { }

void DelayManager::begin()
{
  lastTime = (unsigned int)(millis() % 65536);
}

void DelayManager::sleepReamingOf(unsigned int timeOfExecute)
{
  unsigned int deltatime = getDeltaTime();

  if (deltatime < timeOfExecute)
  {
    delay(timeOfExecute-deltatime);
    lastTime += timeOfExecute;
  }
  else
  {
    //FIXME probably something go really wrong with time consume so manage this
    Serial.print(" ERROR ");
    Serial.print(deltatime);
    Serial.print(" ERROR ");
    lastTime = (unsigned int)(millis() % 65536);
  }
}

unsigned int DelayManager::getDeltaTime()
{
  return (unsigned int)(millis() % 65536) - lastTime;
}