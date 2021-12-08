#include "src/DelayManager/DelayManager.h"
#include "src/Backstore/Backstore.h"
#include "src/TempAndHum/TempAndHum.h"
#include "src/TempControl/TempControl.h"
#include "src/RelayController/RelayController.h"
#include "src/GUI/GUI.h"

#define TEMPSENSORCOUNT 1
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
Backstore store;
TempAndHum tempAndHum;
TempControl tempControl;
RelayController relayController;
GUI gui;

bool needReload = false;

float *TempSensors[TEMPSENSORCOUNT];
float *HumSensors[HUMSENSORCOUNT];

void setup()
{
  delayer.begin();
  store.begin();
  tempAndHum.begin(2);
  relayController.begin(&tempControl);
  gui.begin(&needReload, &store, TempSensors, HumSensors);

  Serial.begin(9600);

  TempSensors[0] = &tempAndHum.temperature;
  HumSensors[0] = &tempAndHum.humidity;

  tempControl.begin(TempSensors, &store); //inicialise should be earlier than this
}

void loop()
{
  Serial.print("\n");
  checkReload();
  
  unsigned int deltatime = delayer.getDeltaTime();
  checkReload();
  tempAndHum.refresh();
  tempControl.refresh(deltatime);
  relayController.refresh(deltatime);
  gui.refresh(deltatime);
  
  //CODE GOES HERE

  gui.endrefresh();
  delayer.sleepReamingOf(50);
}

void checkReload()
{
  if (!needReload) { return; }
  Serial.print("\nRELOAD\n");
  tempControl.readConfig();
  needReload = false;
}
