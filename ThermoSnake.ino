#include "src/DelayManager/DelayManager.h"
#include "src/PT100/PT100.h"

DelayManager delayer;
PT100 pt100;

void setup()
{
  Serial.begin(9600);
  delayer.begin();
  pt100.begin();
}

void loop()
{
  pt100.refresh();
  
  Serial.print("\n");
  Serial.print(pt100.getCurrentTemperature());
  Serial.print(" "); 
  Serial.print(pt100.temperature);
  Serial.print(" "); 
  Serial.print(pt100.temperatureRange);
  Serial.print(" ");
  
  delayer.sleepReamingOf(100);
}
