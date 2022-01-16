#include "src/DelayManager/DelayManager.h"
#include "src/Backstore/Backstore.h"
#include "src/TempAndHum/TempAndHum.h"
#include "src/TempControl/TempControl.h"
#include "src/RelayController/RelayController.h"

#define CHANNEL_COUNT 2 //max 255
#define TEMPSENSORCOUNT 2
#define HUMSENSORCOUNT 2

DelayManager delayer;
Backstore store;
TempAndHum tempAndHum;
TempAndHum tempAndHum2;
TempControl tempControl;
RelayController relayController;

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

bool needReload = false;

float *TempSensors[TEMPSENSORCOUNT];
float *HumSensors[HUMSENSORCOUNT];

void setup()
{
  Serial.begin(9600);

  store.begin();
  tempAndHum.begin(2, &store);
  tempAndHum2.begin(8, &store);

  TempSensors[0] = &tempAndHum1.temperature;
  TempSensors[1] = &tempAndHum2.temperature;
  HumSensors[0] = &tempAndHum1.humidity;
  HumSensors[1] = &tempAndHum2.humidity;

  tempControl.begin(TempSensors, &store);
  relayController.begin(&store, &tempControl);
  delayer.begin();
}

float presstime = 0;
void loop()
{
  unsigned int deltatime = delayer.getDeltaTime();

  //DEBUG
  Serial.print("\n");
  checkReload();

  tempAndHum.refresh();
  tempAndHum2.refresh();
  tempControl.refresh(deltatime);
  relayController.refresh(deltatime);
  
  //DEBUG
  Serial.print(" - STAT - ");
  Serial.print(relayController.combo);
  Serial.print(" ");
  Serial.print(relayController.ch0);
  Serial.print(" ");
  Serial.print(relayController.ch1);
  Serial.print(" - CH 0 - ");
  Serial.print(TempSensors[0], DEC);
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
  Serial.print(TempSensors[1], DEC);
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

  delayer.sleepReamingOf(50);
}

void checkReload()
{
  if (!needReload) { return; }

  Serial.print("\nRELOAD\n");
  tempAndHum.readConfig();
  tempAndHum2.readConfig();

  needReload = false;
}
