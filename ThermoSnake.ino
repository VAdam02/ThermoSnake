#include "src/DelayManager/DelayManager.h"

DelayManager delayer;

void setup()
{
  Serial.begin(9600);
  
  delayer.begin();
}

void loop()
{
  //Code goes here
  
  delayer.sleepReamingOf(50);
}
