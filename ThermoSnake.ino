#include "src/TempAndHum/TempAndHum.h"

TempAndHum tempAndHum;

long time = 1;

void setup()
{
  Serial.begin(9600);
  tempAndHum.begin();
}

void loop()
{
  tempAndHum.refresh(time);

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
  
  delay(50);
  time += 50;
}
