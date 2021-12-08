#include "src/DelayManager/DelayManager.h"
#include "src/TempAndHum/TempAndHum.h"

DelayManager delayer;
TempAndHum tempAndHum;
//TempAndHum tempAndHum2;

void setup()
{
  Serial.begin(9600);
  delayer.begin();
  tempAndHum.begin(2);
  //tempAndHum2.begin(8);
}

void loop()
{
  tempAndHum.refresh();
  //tempAndHum2.refresh();

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

  /*
  Serial.print("  -  ");
  Serial.print(tempAndHum2.getCurrentTemperature());
  Serial.print(" "); 
  Serial.print(tempAndHum2.temperature);
  Serial.print(" "); 
  Serial.print(tempAndHum2.temperatureRange);
  Serial.print(" ");
  
  Serial.print(tempAndHum2.getCurrentHumidity());
  Serial.print(" "); 
  Serial.print(tempAndHum2.humidity);
  Serial.print(" "); 
  Serial.print(tempAndHum2.humidityRange);
  */
  
  delayer.sleepReamingOf(2000);
}
