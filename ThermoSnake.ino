#include "src/DelayManager/DelayManager.h"
#include "src/TempAndHum/TempAndHum.h"
#include "src/Backstore/Backstore.h"
#include "src/TempControl/TempControl.h"

DelayManager delayer;
TempAndHum tempAndHum;
Backstore store;
TempControl tempControl;

void setup()
{
  Serial.begin(9600);
  delayer.begin();
  tempAndHum.begin();
  store.begin();
  //store.mem();

  
  store.freeUpSpace('B', 1);
  store.allocateSpace('B', 1, 9);
  //TODO double -> byte

  byte data[2];
  byte demo[] = { 2, 201, 201, 202, 202, 203, 203, 10, 60 };
  
  tempControl.getByteFormat(30, data);
  demo[1] = data[0];
  demo[2] = data[1];

  tempControl.getByteFormat(10, data);
  demo[3] = data[0];
  demo[4] = data[1];

  tempControl.getByteFormat(1.3, data);
  demo[5] = data[0];
  demo[6] = data[1];
  
  store.writeBytes('B', 1, 0, 8, demo);
  store.mem();
  tempControl.begin(&tempAndHum, &store);
  
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

double presstime = 0;
double data = 0;
unsigned int lastTime = 0;
void loop()
{
  tempAndHum.refresh();
  tempControl.refresh();

  
  double x = analogRead(A0);
  double y = analogRead(A1);
  if ((x-512) > 200)
  {
    Serial.print("RON - ");
    tempControl.chanelParams[1][0] = 3;
    tempControl.chanelParams[1][5] = 0;
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
      
      if (tempControl.chanelParams[1][2] < (presstime/1000) * deltatime/1000)
      {
        tempControl.chanelParams[1][2] = 0;
      }
      else
      {
        tempControl.chanelParams[1][2] -= (presstime/1000) * deltatime /1000;
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
    float range = 0;
    
    range = 0;
    //Serial.print(tempAndHum.getTemperature(&range));
    Serial.print(data);
    Serial.print(" - ");
    Serial.print(tempControl.level1(1, data, 100));
    Serial.print(" - State ");
    Serial.print(tempControl.chanelParams[1][0]);
    Serial.print(" - OnTime ");
    Serial.print(tempControl.chanelParams[1][2]);
    Serial.print(" - Cool ");
    Serial.print(tempControl.chanelParams[1][3]);
    Serial.print(" ");
    Serial.print(tempControl.chanelParams[1][4]);
    Serial.print(" - Delay ");
    Serial.print(tempControl.chanelParams[1][5]);
    Serial.print(" - CurrentState ");
    Serial.print(tempControl.chanelParams[1][14]);
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
  
  delayer.sleepReamingOf(50);
}
