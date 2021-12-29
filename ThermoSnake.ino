#include "src/Watchdog/Watchdog.h"
#include "src/DelayManager/DelayManager.h"
#include "src/TempAndHum/TempAndHum.h"
#include "src/Backstore/Backstore.h"
#include "src/TempControl/TempControl.h"
#include "src/RelayController/RelayController.h"
#include "src/GUI/GUI.h"

#define TEMPSENSORCOUNT 4
#define HUMSENSORCOUNT 2

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

//Watchdog wdog;
DelayManager delayer;
Backstore store;
TempAndHum tempAndHum1;
TempAndHum tempAndHum2;
TempControl tempControl;
RelayController relayController;
GUI gui;

bool needReload = false;

float *TempSensors[TEMPSENSORCOUNT];
float *HumSensors[HUMSENSORCOUNT];

float value0 = 24.75;
float heating0 = 0.1; //after 1 second
float cooling0 = 0.01; //after 1 second

float value1 = 31;
float heating1 = 0.1; //after 1 second
float cooling1 = 0.01; //after 1 second

void setup()
{
  Serial.begin(9600);
  delayer.begin();
  store.begin();
  tempAndHum1.begin(2, &store);
  tempAndHum2.begin(5, &store);
  relayController.begin(&store, &tempControl);
  gui.begin(&needReload, &store, TempSensors, HumSensors);

  TempSensors[0] = &tempAndHum1.temperature;
  TempSensors[1] = &tempAndHum2.temperature;
  TempSensors[2] = &value0;
  TempSensors[3] = &value1;
  HumSensors[0] = &tempAndHum1.humidity;
  HumSensors[1] = &tempAndHum2.humidity;

  store.mem();

  tempControl.begin(TempSensors, &store); //inicialise should be earlier than this
  //store.inicialise(256);

  //wdog.begin(&store, TEMPSENSORCOUNT, TempSensors, HUMSENSORCOUNT, HumSensors);
}

void loop()
{
  Serial.print("\n");
  unsigned int deltatime = delayer.getDeltaTime();
  
  checkReload();
  //wdog.refresh(deltatime);
  tempAndHum1.refresh();
  tempAndHum2.refresh();
  tempControl.refresh(deltatime);
  relayController.refresh(deltatime);
  gui.refresh(deltatime);

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
  
  gui.endrefresh();
  delayer.sleepReamingOf(50);
}

void checkReload()
{
  if (!needReload) { return; }
  Serial.print("\nRELOAD\n");
  tempAndHum1.readConfig();
  tempAndHum2.readConfig();
  tempControl.readConfig();
  needReload = false;
}

String numToString(double num, int decimals)
{
  String data = "";
  int greater = num;
  num -= greater;
  while (greater > 0)
  {
    data = String(greater % 10) + data;
    greater /= 10;
  }

  if (decimals > 0) { data += "."; }

  for (int i = 0; i < decimals; i++)
  {
    num *= 10;
    int cache = (int)num;
    data = data + String(cache);
    num -= cache;
  }
  
  return data;
}
