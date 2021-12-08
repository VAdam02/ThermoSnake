#include "src/Backstore/Backstore.h"
#include "src/DelayManager/DelayManager.h"
#include "src/TempAndHum/TempAndHum.h"

Backstore store;
DelayManager delayer;
TempAndHum tempAndHum;

void setup()
{
  Serial.begin(9600);
  Backstore.begin();
  delayer.begin();
  tempAndHum.begin(2);
}

void loop()
{
  tempAndHum.refresh();

  Serial.print("\n");
  Serial.print(tempAndHum.getCurrentTemperature());
  Serial.print(" "); 
  Serial.print(tempAndHum.temperature);
  Serial.print(" "); 
  Serial.print(tempAndHum.temperatureRange);
  Serial.print(" ");
  
  Serial.print(tempAndHum.getCurrentHumidity());
  Serial.print(" "); 
  Serial.print(tempAndHum.humidity);
  Serial.print(" "); 
  Serial.print(tempAndHum.humidityRange);
  
  delayer.sleepReamingOf(2000);
}
