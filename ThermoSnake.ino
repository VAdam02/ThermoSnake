#include "src/DelayManager/DelayManager.h"
#include "src/TempAndHum/TempAndHum.h"
#include "src/Backstore/Backstore.h"
#include "src/TempControl/TempControl.h"
#include "src/RelayController/RelayController.h"

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
#define LEVEL1_TOLERANCE 11
#define LEVEL1_TOLERANCE2 12
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
RelayController relayController;

float *TempSensors[TEMPSENSORCOUNT];
float *HumSensors[HUMSENSORCOUNT];

float value0 = 25;
float heating0 = 0.1; //after 1 second
float cooling0 = 0.01; //after 1 second

float value1 = 31;
float heating1 = 0.1; //after 1 second
float cooling1 = 0.01; //after 1 second

void setup()
{
  Serial.begin(9600);
  delayer.begin();
  tempAndHum.begin(2);
  store.begin();
  relayController.begin(&store, &tempControl);

  TempSensors[0] = &tempAndHum.temperature;
  TempSensors[1] = &value0;
  TempSensors[2] = &value1;
  HumSensors[0] = &tempAndHum.humidity;

  //store.mem();

  tempControl.begin(TempSensors, &store); //inicialise should be earlier than this
  //maybe there's an error due to not reading config
}

float presstime = 0;
void loop()
{
  unsigned int deltatime = delayer.getDeltaTime();
  
  //DEBUG
  Serial.print("\n");
  tempAndHum.refresh();
  tempControl.refresh(deltatime);
  relayController.refresh(deltatime);

  //Temperature cooling
  value0 -= ((float)(deltatime)/1000)*cooling0;
  value1 -= ((float)(deltatime)/1000)*cooling1;
  //Temperature cooling

  //ch 0 heat
  if (tempControl.channelParams[0][LEVELX_STATE] == 3)
  {
    value0 += ((float)(deltatime)/1000)*heating0;
  }

  //ch 1 heat
  if (tempControl.channelParams[1][LEVELX_STATE] == 3)
  {
    value1 += ((float)(deltatime)/1000)*heating1;
  }
  
  //DEBUG
  Serial.print(" - STAT - ");
  Serial.print(relayController.combo);
  Serial.print(" ");
  Serial.print(relayController.ch0);
  Serial.print(" ");
  Serial.print(relayController.ch1);
  Serial.print(" - CH 0 - ");
  Serial.print(value0, DEC);
  Serial.print(" - State ");
  Serial.print(tempControl.channelParams[0][LEVELX_STATE]);
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
  Serial.print(tempControl.channelParams[1][LEVELX_STATE]);
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

  delayer.sleepReamingOf(50);
}
