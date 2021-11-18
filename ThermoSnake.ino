#include "src/DelayManager/DelayManager.h"
#include "src/Backstore/Backstore.h"
#include "src/TempAndHum/TempAndHum.h"
#include "src/TempControl/TempControl.h"
#include "src/RelayController/RelayController.h"
#include "src/GUI/GUI.h"

#define TEMPSENSORCOUNT 3
#define HUMSENSORCOUNT 1

#define CHANNEL_COUNT 2 //max 255

DelayManager delayer;
Backstore store;
TempControl tempControl;
TempAndHum tempAndHum;
RelayController relayController;
GUI gui;

bool needReload = false;

float value0 = 25;
float value1 = 25;

float *TempSensors[TEMPSENSORCOUNT];
float *HumSensors[HUMSENSORCOUNT];

void setup()
{
  Serial.begin(9600);
  delayer.begin();
  store.begin();
  tempAndHum.begin();
  relayController.begin(&tempControl);
  gui.begin(&needReload, &store, TempSensors, HumSensors);

  TempSensors[0] = &tempAndHum.temperature;
  TempSensors[1] = &value0;
  TempSensors[1] = &value1;
  HumSensors[0] = &tempAndHum.humidity;

  tempControl.begin(TempSensors, &store); //inicialise should be earlier than this
  lastTime = millis();
}

void loop()
{
  Serial.print("\n");
  checkReload();

  unsigned int deltatime = delayer.getDeltaTime();

  tempAndHum.refresh();
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

  if (deltatime > 0) //DEBUG
  {
    //Serial.print(tempAndHum.getTemperature(&range));
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
  }

  gui.endrefresh();
  delayer.sleepReamingOf(50);
}

void checkReload()
{
  if (!needReload) { return; }

  tempControl.readConfig();
}
