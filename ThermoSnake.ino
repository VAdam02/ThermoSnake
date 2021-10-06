#include "src/Buzzer/Buzzer.h"

Buzzer buzzer;

long time = 1;

void setup()
{
  buzzer.begin(4);
}

void loop()
{
  buzzer.refresh(time);
  
  delay(50);
  time += 50;
}
