#include "DelayManager.h"
#include <Arduino.h>

DelayManager::DelayManager() { }

int Delay_lastTime = 0;
void DelayManager::begin()
{
    Delay_lastTime = millis();
}

void DelayManager::sleepReamingOf(int timeOfExecute)
{
    //calculate deltatime
    int deltatime = (int)(millis() % 32768);
    if (deltatime <= Delay_lastTime)
    {
        Delay_lastTime = 0 - (32767 - Delay_lastTime);
    }
    deltatime = deltatime - Delay_lastTime;
    //calculate deltatime

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