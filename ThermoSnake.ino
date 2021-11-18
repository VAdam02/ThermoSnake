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

unsigned int lastTime = 0;
void loop()
{
  tempAndHum.refresh();


  unsigned int deltatime = (unsigned int)(millis() % 65536) - lastTime;
  if (deltatime > 2500)
  {
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
    
    lastTime += 2500;
  }

  delayer.sleepReamingOf(50);
}
