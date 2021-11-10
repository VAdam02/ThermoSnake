#include "src/DelayManager/DelayManager.h"
#include "src/TempAndHum/TempAndHum.h"
#include "src/Backstore/Backstore.h"
#include "src/TempControl/TempControl.h"

#define TEMPSENSORCOUNT 3
#define HUMSENSORCOUNT 1

#define CHANNEL_COUNT 2 //max 255

//GENERAL
#define LEVELX_STATE 0
#define LEVELX_MODE 1
#define LEVELX_SENSOR_ID 2
#define LEVELX_ONTIME_LEFT 3
#define LEVELX_ONTIME_LEFT2 4
#define LEVELX_COOLDOWN_LEFT 5
#define LEVELX_COOLDOWN_LEFT2 6
#define LEVELX_MAXDELAY_LEFT 7
#define LEVELX_MAXDELAY_LEFT2 8

//LEVEL 0
#define LEVEL0_OFFLEVEL 9
#define LEVEL0_OFFLEVEL2 10
#define LEVEL0_ONLEVEL 11
#define LEVEL0_ONLEVEL2 12

//LEVEL 1
#define LEVEL1_TARGETLEVEL 9
#define LEVEL1_TARGETLEVEL2 10
#define LEVEL1_DIFFERENCE 11
#define LEVEL1_DIFFERENCE2 12
#define LEVEL1_REACTION 13
#define LEVEL1_REACTION2 14
#define LEVEL1_MINON 15
#define LEVEL1_MAXON 16
#define LEVEL1_CURRENTSTATE 17
#define LEVEL1_SAMPLE_TEMPERATURE 18
#define LEVEL1_SAMPLE_TEMPERATURE2 19
#define LEVEL1_SAMPLE_POWERONTIME 20

DelayManager delayer;
TempAndHum tempAndHum;
Backstore store;
TempControl tempControl;

unsigned int ch0 = 0;
unsigned int ch1 = 0;
unsigned int combo = 0;

float *TempSensors[TEMPSENSORCOUNT];
float *HumSensors[HUMSENSORCOUNT];

float value0 = 25;
float heating0 = 0.1; //after 1 second
float cooling0 = 0.01; //after 1 second

float value1 = 30;
float heating1 = 0.1; //after 1 second
float cooling1 = 0.01; //after 1 second

byte *relay[CHANNEL_COUNT];

unsigned int lastTime = 0;

void setup()
{
  Serial.begin(9600);
  delayer.begin();
  tempAndHum.begin();
  store.begin();

  TempSensors[0] = &tempAndHum.temperature;
  TempSensors[1] = &value0;
  TempSensors[2] = &value1;
  HumSensors[0] = &tempAndHum.humidity;
  //store.mem();

  //Inicialise
  store.freeUpSpace('B', 0);
  store.freeUpSpace('B', 1);

  byte data[2];
  
  //channel 0
  store.allocateSpace('B', 0, 10);
  byte demo0[] = { 1, 1, 201, 201, 202, 202, 0, 0, 0, 0 };
  
  tempControl.getByteFormat(28, data);
  demo0[2] = data[0];
  demo0[3] = data[1];

  tempControl.getByteFormat(26, data);
  demo0[4] = data[0];
  demo0[5] = data[1];
  
  store.writeBytes('B', 0, 0, 9, demo0);

  //channel 1
  store.allocateSpace('B', 1, 10);
  byte demo1[] = { 2, 2, 201, 201, 202, 202, 203, 203, 10, 60 };
  
  tempControl.getByteFormat(32.5, data);
  demo1[2] = data[0];
  demo1[3] = data[1];

  tempControl.getUnsignedByteFormat(1, data);
  demo1[4] = data[0];
  demo1[5] = data[1];

  tempControl.getByteFormat(5, data);
  demo1[6] = data[0];
  demo1[7] = data[1];
  
  store.writeBytes('B', 1, 0, 9, demo1);

  tempControl.getRelayStates(2, relay);
  //store.mem();

  tempControl.begin(TempSensors, &store); //TODO inicialise should be earlier than this
  //TODO should be started 1 minute after the sensors
  //maybe there's an error due to not reading config

  lastTime = millis();
}

float presstime = 0;
void loop()
{
  //DEBUG
  Serial.print("\n");
  tempAndHum.refresh();
  tempControl.refresh();

  unsigned int delta = 50;
  byte data[2];

  //maxDelayLeft
  data[0] = tempControl.channelParams[0][LEVELX_MAXDELAY_LEFT];
  data[1] = tempControl.channelParams[0][LEVELX_MAXDELAY_LEFT2];
  float maxDelayLeft0 = tempControl.reverseUnsignedByteFormat(data);
  if (maxDelayLeft0 > 0 && tempControl.channelParams[0][LEVELX_STATE] == 2)
  {
    if (maxDelayLeft0 < ((float)(delta)/1000)) { maxDelayLeft0 = 0; }
    else { maxDelayLeft0 -= ((float)(delta)/1000); }
  }
  tempControl.getUnsignedByteFormat(maxDelayLeft0, data);
  tempControl.channelParams[0][LEVELX_MAXDELAY_LEFT] = data[0];
  tempControl.channelParams[0][LEVELX_MAXDELAY_LEFT2] = data[1];
  
  data[0] = tempControl.channelParams[1][LEVELX_MAXDELAY_LEFT];
  data[1] = tempControl.channelParams[1][LEVELX_MAXDELAY_LEFT2];
  float maxDelayLeft1 = tempControl.reverseUnsignedByteFormat(data);
  if (maxDelayLeft1 > 0 && tempControl.channelParams[1][LEVELX_STATE] == 2)
  {
    if (maxDelayLeft1 < ((float)(delta)/1000)) { maxDelayLeft1 = 0; }
    else { maxDelayLeft1 -= ((float)(delta)/1000); }
  }
  tempControl.getUnsignedByteFormat(maxDelayLeft1, data);
  tempControl.channelParams[1][LEVELX_MAXDELAY_LEFT] = data[0];
  tempControl.channelParams[1][LEVELX_MAXDELAY_LEFT2] = data[1];
  //maxDelayLeft


  
  //onTimeLeft
  data[0] = tempControl.channelParams[0][LEVELX_ONTIME_LEFT];
  data[1] = tempControl.channelParams[0][LEVELX_ONTIME_LEFT2];
  float onTimeLeft0 = tempControl.reverseUnsignedByteFormat(data);
  
  if (0 < onTimeLeft0 && tempControl.channelParams[0][LEVELX_STATE] == 3)
  {
    value0 += ((float)(delta)/1000)*heating0;
    if (onTimeLeft0 < 255)
    {
      if (onTimeLeft0 < ((float)(delta)/1000)) { onTimeLeft0 = 0; }
      else { onTimeLeft0 -= ((float)(delta)/1000); }
    }
  }
  
  tempControl.getUnsignedByteFormat(onTimeLeft0, data);
  tempControl.channelParams[0][LEVELX_ONTIME_LEFT] = data[0];
  tempControl.channelParams[0][LEVELX_ONTIME_LEFT2] = data[1];
  
  data[0] = tempControl.channelParams[1][LEVELX_ONTIME_LEFT];
  data[1] = tempControl.channelParams[1][LEVELX_ONTIME_LEFT2];
  float onTimeLeft1 = tempControl.reverseUnsignedByteFormat(data);
  if (0 < onTimeLeft1 && tempControl.channelParams[1][LEVELX_STATE] == 3)
  {
    value1 += ((float)(delta)/1000)*heating1;
    if (onTimeLeft1 < 255)
    {
      if (onTimeLeft1 < ((float)(delta)/1000)) { onTimeLeft1 = 0; }
      else { onTimeLeft1 -= ((float)(delta)/1000); }
    }
  }
  tempControl.getUnsignedByteFormat(onTimeLeft1, data);
  tempControl.channelParams[1][LEVELX_ONTIME_LEFT] = data[0];
  tempControl.channelParams[1][LEVELX_ONTIME_LEFT2] = data[1];
  //onTimeLeft



  //Temperature cooling
  value0 -= ((float)(delta)/1000)*cooling0;
  value1 -= ((float)(delta)/1000)*cooling1;
  //Temperature cooling



  //both need change
  if (((maxDelayLeft0 >= 0 && tempControl.channelParams[0][LEVELX_STATE] == 2) || (onTimeLeft0 == 0 && tempControl.channelParams[0][LEVELX_STATE] == 0)) &&
      ((maxDelayLeft1 >= 0 && tempControl.channelParams[1][LEVELX_STATE] == 2) || (onTimeLeft1 == 0 && tempControl.channelParams[1][LEVELX_STATE] == 0)))
  {
    Serial.print(" _ COMBO _ ");
    combo++;
    //0 channel
    //on
    if (maxDelayLeft0 >= 0 && tempControl.channelParams[0][LEVELX_STATE] == 2)
    {
      digitalWrite(3, HIGH);
      tempControl.channelParams[0][LEVELX_STATE] = 3;
      
      tempControl.getUnsignedByteFormat(0, data);
      tempControl.channelParams[0][LEVELX_MAXDELAY_LEFT] = data[0];
      tempControl.channelParams[0][LEVELX_MAXDELAY_LEFT2] = data[1];
    }
    //off
    else
    {
      if (onTimeLeft0 == 0 && tempControl.channelParams[0][LEVELX_STATE] == 3)
      {
        tempControl.channelParams[0][LEVELX_STATE] = 0;
      }
      if (tempControl.channelParams[0][LEVELX_STATE] == 0)
      {
        tempControl.channelParams[0][LEVELX_STATE] = 1;
        digitalWrite(3, LOW);
      }
    }

    //1 channel
    //on
    if (maxDelayLeft1 >= 0 && tempControl.channelParams[1][LEVELX_STATE] == 2)
    {
      digitalWrite(2, HIGH);
      tempControl.channelParams[1][LEVELX_STATE] = 3;

      tempControl.getUnsignedByteFormat(0, data);
      tempControl.channelParams[1][LEVELX_MAXDELAY_LEFT] = data[0];
      tempControl.channelParams[1][LEVELX_MAXDELAY_LEFT2] = data[1];
    }
    //off
    else
    {
      if (onTimeLeft1 == 0 && tempControl.channelParams[1][LEVELX_STATE] == 3)
      {
        tempControl.channelParams[1][LEVELX_STATE] = 0;
      }
      if (tempControl.channelParams[1][LEVELX_STATE] == 0)
      {
        tempControl.channelParams[1][LEVELX_STATE] = 1;
        digitalWrite(2, LOW);
      }
    }
  }
  //only one need change
  else
  {
    //0 channel
    //on
    if (maxDelayLeft0 == 0 && tempControl.channelParams[0][LEVELX_STATE] == 2)
    {
      ch0++;
      digitalWrite(3, HIGH);
      tempControl.channelParams[0][LEVELX_STATE] = 3;
    }
    //off
    else
    {
      if (onTimeLeft0 == 0 && tempControl.channelParams[0][LEVELX_STATE] == 3)
      {
        tempControl.channelParams[0][LEVELX_STATE] = 0;
      }
      if (tempControl.channelParams[0][LEVELX_STATE] == 0)
      {
        tempControl.channelParams[0][LEVELX_STATE] = 1;
        digitalWrite(3, LOW);
        ch0++;
      }
    }

    //1 channel
    //on
    if (maxDelayLeft1 == 0 && tempControl.channelParams[1][LEVELX_STATE] == 2)
    {
      ch1++;
      digitalWrite(2, HIGH);
      tempControl.channelParams[1][LEVELX_STATE] = 3;
    }
    //off
    else
    {
      if (onTimeLeft1 == 0 && tempControl.channelParams[1][LEVELX_STATE] == 3)
      {
        tempControl.channelParams[1][LEVELX_STATE] = 0;
      }
      if (tempControl.channelParams[1][LEVELX_STATE] == 0)
      {
        tempControl.channelParams[1][LEVELX_STATE] = 1;
        digitalWrite(2, LOW);
        ch1++;
      }
    }
  }


  
  /*
  double x = analogRead(A0);
  double y = analogRead(A1);
  if ((x-512) > 200)
  {
    Serial.print("RON - ");
    tempControl.channelParams[1][0] = 3;
    tempControl.channelParams[1][6] = 0;
  }
  if ((x-512) < -200)
  {
    Serial.print("ROFF - ");
    tempControl.channelParams[1][0] = 1;
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
      
      if (tempControl.channelParams[1][3] < (presstime/1000) * deltatime/1000)
      {
        tempControl.channelParams[1][3] = 0;
      }
      else
      {
        tempControl.channelParams[1][3] -= (presstime/1000) * deltatime /1000;
      }
      
    }
    Serial.print(data);
    Serial.print("\n");
  }
  else
  {
    presstime = 0;
  }
  */
  
  unsigned int deltatime = (unsigned int)(millis() % 65536) - lastTime;
  if (deltatime > 1000)
  {
    //Serial.print(tempAndHum.getTemperature(&range));
    Serial.print(" - STAT - ");
    Serial.print(combo);
    Serial.print(" ");
    Serial.print(ch0);
    Serial.print(" ");
    Serial.print(ch1);
    Serial.print(" - CH 0 - ");
    Serial.print(value0, DEC);
    Serial.print(" - State ");
    Serial.print(*relay[0]);
    //Serial.print(tempControl.channelParams[0][LEVELX_STATE]);
    Serial.print(" - OnTime ");
    Serial.print(tempControl.channelParams[0][LEVELX_ONTIME_LEFT]);
    Serial.print(" ");
    Serial.print(tempControl.channelParams[0][LEVELX_ONTIME_LEFT2]);
    Serial.print(" - Delay ");
    Serial.print(tempControl.channelParams[0][LEVELX_MAXDELAY_LEFT]);
    Serial.print(" ");
    Serial.print(tempControl.channelParams[0][LEVELX_MAXDELAY_LEFT2]);
    Serial.print(" - CH 1 - ");
    Serial.print(value1, DEC);
    Serial.print(" - State ");
    Serial.print(*relay[1]);
    //Serial.print(tempControl.channelParams[1][LEVELX_STATE]);
    Serial.print(" - OnTime ");
    Serial.print(tempControl.channelParams[1][LEVELX_ONTIME_LEFT]);
    Serial.print(" ");
    Serial.print(tempControl.channelParams[1][LEVELX_ONTIME_LEFT2]);
    Serial.print(" - Cool ");
    Serial.print(tempControl.channelParams[1][LEVELX_COOLDOWN_LEFT]);
    Serial.print(" ");
    Serial.print(tempControl.channelParams[1][LEVELX_COOLDOWN_LEFT2]);
    Serial.print(" - Delay ");
    Serial.print(tempControl.channelParams[1][LEVELX_MAXDELAY_LEFT]);
    Serial.print(" ");
    Serial.print(tempControl.channelParams[1][LEVELX_MAXDELAY_LEFT2]);
    Serial.print(" - CurrentState ");
    Serial.print(tempControl.channelParams[1][LEVEL1_CURRENTSTATE]);
    Serial.print("\n");

    lastTime += 1000;
  }

  /*
  //TODO make better debug
  if (tempControl.channelParams[1][0] == 3)
  {
    digitalWrite(2, HIGH);
  }
  else if (tempControl.channelParams[1][0] == 1)
  {
    digitalWrite(2, LOW);
  }
  */
  delayer.sleepReamingOf(50);
}
