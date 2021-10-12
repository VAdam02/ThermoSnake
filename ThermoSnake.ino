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

void loop()
{
  tempAndHum.refresh();

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

  delayer.sleepReamingOf(50);
}
