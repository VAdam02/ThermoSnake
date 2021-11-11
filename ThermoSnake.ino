#include "src/DelayManager/DelayManager.h"
#include "src/Buzzer/Buzzer.h"

DelayManager delayer;
Buzzer buzzer;

void setup()
{
  Serial.begin(9600);
  delayer.begin();
  buzzer.begin(7);
  delay(500);
  buzzer.sayError();
}

void loop()
{
  buzzer.refresh();
  
  delayer.sleepReamingOf(50);
}
