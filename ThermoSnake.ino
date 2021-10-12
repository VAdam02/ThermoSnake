#include "src/DelayManager/DelayManager.h"
#include "src/Buzzer/Buzzer.h"

DelayManager delayer;
Buzzer buzzer;

void setup()
{
  delayer.begin();
  buzzer.begin(4);
}

void loop()
{
  buzzer.refresh(time);
  
  delayer.sleepReamingOf(50);
}
