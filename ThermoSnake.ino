#include "src/DelayManager/DelayManager.h"
#include "src/TempAndHum/TempAndHum.h"

DelayManager delayer;
TempAndHum tempAndHum;

void setup()
{
  Serial.begin(9600);
  delayer.begin();
  tempAndHum.begin();
}

double offLevel = 25.4;
double onLevel = 25.3;
/*
 * 0 - Turn off
 * 1 - Turned off
 * 2 - Turn on
 * 3 - Turned on
 */
byte state = 0;
unsigned int lastTime = 0;

/* 
 * RETURN VALUE
 * True - new command
 * False - no new command
 */
bool level0(double offLevel, double onLevel, double curLevel, byte* state)
{
  if (curLevel < onLevel)
  {
    if (*state == 0 || *state == 1)
    {
      *state = 2;
      return true; 
    }
    return false;
    //on
  }
  else if (offLevel < curLevel)
  {
    if (*state == 2 || *state == 3)
    {
      *state = 0;
      return true; 
    }
    return false;
    //off
  }
}

void loop()
{
  tempAndHum.refresh();

  
  
  unsigned int deltatime = (unsigned int)(millis() % 65536) - lastTime;
  if (deltatime > 1000)
  {
    float range = 0;
    Serial.print(tempAndHum.getTemperature(&range));
    Serial.print(" - ");
    Serial.print(level0(offLevel, onLevel, tempAndHum.getTemperature(&range), &state));
    Serial.print(" - ");
    Serial.print(state);
    Serial.print("\n");

    /*
    Serial.print("\n");
    float range = 0;
    float val = tempAndHum.getTemperature(&range);
    Serial.print(tempAndHum.getCurrentTemperature());
    Serial.print(" "); 
    Serial.print(val);
    Serial.print(" "); 
    Serial.print(range);
    Serial.print(" ");
    val = tempAndHum.getHumidity(&range);
    Serial.print(tempAndHum.getCurrentHumidity());
    Serial.print(" "); 
    Serial.print(val);
    Serial.print(" "); 
    Serial.print(range);
    */
    lastTime += 1000;
  }

  delayer.sleepReamingOf(50);
}
