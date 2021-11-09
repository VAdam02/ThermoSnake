#include "src/DelayManager/DelayManager.h"
#include "src/TempAndHum/TempAndHum.h"
#include "src/Backstore/Backstore.h"
#include "src/TempControl/TempControl.h"

#define TEMPSENSORCOUNT 2
#define HUMSENSORCOUNT 1

DelayManager delayer;
TempAndHum tempAndHum;
Backstore store;
TempControl tempControl;

float *TempSensors[TEMPSENSORCOUNT];
float *HumSensors[HUMSENSORCOUNT];

float data = 0;

void setup()
{
  Serial.begin(9600);
  delayer.begin();
  tempAndHum.begin();
  store.begin();

  TempSensors[0] = &tempAndHum.temperature;
  TempSensors[1] = &data;
  HumSensors[0] = &tempAndHum.humidity;
  tempControl.begin(TempSensors, &store);
  //store.mem();

  store.freeUpSpace('B', 0);
  store.freeUpSpace('B', 1);
  store.allocateSpace('B', 1, 10);
  //TODO double -> byte

  byte data[2];
  byte demo[] = { 2, 1, 201, 201, 202, 202, 203, 203, 10, 60 };
  
  tempControl.getByteFormat(30, data);
  demo[2] = data[0];
  demo[3] = data[1];

  tempControl.getByteFormat(10, data);
  demo[4] = data[0];
  demo[5] = data[1];

  tempControl.getByteFormat(1.3, data);
  demo[6] = data[0];
  demo[7] = data[1];
  
  store.writeBytes('B', 1, 0, 9, demo);
  //store.mem();
}

/*
unsigned int getHeatingTaskTime(byte ID)
{
  return on_times[ID];
}
void addHeatingTask (byte ID, unsigned int on_time, unsigned int maxDelay_time)
{
  on_times[ID] = on_time;
  if (maxDelay_times[ID] == 0 || maxDelay_times[ID] > maxDelay_time)
  {
    maxDelay_times[ID] = maxDelay_time;
  }
}
*/

float presstime = 0;
unsigned int lastTime = 0;
void loop()
{
  //DEBUG
  Serial.print("\n");
  tempAndHum.refresh();
  tempControl.refresh();
  
  double x = analogRead(A0);
  double y = analogRead(A1);
  if ((x-512) > 200)
  {
    Serial.print("RON - ");
    tempControl.chanelParams[1][0] = 3;
    tempControl.chanelParams[1][6] = 0;
  }
  if ((x-512) < -200)
  {
    Serial.print("ROFF - ");
    tempControl.chanelParams[1][0] = 1;
  }
  
  if (abs((y-512)/512) > 0.1)
  {
    double deltatime = 50;
    presstime += deltatime;
    
    if ((y-512) > 0)
    {
      data -= (presstime/1000) * deltatime / 1000;
    }
    else
    {
      data += (presstime/1000) * deltatime / 1000;
      
      if (tempControl.chanelParams[1][3] < (presstime/1000) * deltatime/1000)
      {
        tempControl.chanelParams[1][3] = 0;
      }
      else
      {
        tempControl.chanelParams[1][3] -= (presstime/1000) * deltatime /1000;
      }
      
    }
    Serial.print(data);
    Serial.print("\n");
  }
  else
  {
    presstime = 0;
  }
  
  
  unsigned int deltatime = (unsigned int)(millis() % 65536) - lastTime;
  if (deltatime > 1000)
  {
    //Serial.print(tempAndHum.getTemperature(&range));
    Serial.print(data);
    Serial.print(" - State ");
    Serial.print(tempControl.chanelParams[1][0]);
    Serial.print(" - OnTime ");
    Serial.print(tempControl.chanelParams[1][3]);
    Serial.print(" - Cool ");
    Serial.print(tempControl.chanelParams[1][4]);
    Serial.print(" ");
    Serial.print(tempControl.chanelParams[1][5]);
    Serial.print(" - Delay ");
    Serial.print(tempControl.chanelParams[1][6]);
    Serial.print(" - CurrentState ");
    Serial.print(tempControl.chanelParams[1][15]);
    Serial.print("\n");

    lastTime += 1000;
  }

  //TODO make better debug
  if (tempControl.chanelParams[1][0] == 3)
  {
    digitalWrite(2, HIGH);
  }
  else if (tempControl.chanelParams[1][0] == 1)
  {
    digitalWrite(2, LOW);
  }
  
  delayer.sleepReamingOf(250);
}
