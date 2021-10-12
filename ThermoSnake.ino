#include "src/DelayManager/DelayManager.h"

DelayManager delayer;

void setup()
{
  delayer.begin();
}

void loop()
{
  //Code goes here
  
  delayer.sleepReamingOf(50);
}
