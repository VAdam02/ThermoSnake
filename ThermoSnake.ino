#include "src/DelayManager/DelayManager.h"
#include "src/Backstore/Backstore.h"
#include "src/PT100/PT100.h"
#include "src/TempAndHum/TempAndHum.h"
#include "src/TempControl/TempControl.h"
#include "src/RelayController/RelayController.h"
#include "src/GUI/GUI.h"

#define TEMPSENSORCOUNT 2
#define HUMSENSORCOUNT 1

#define CHANNEL_COUNT 2 //max 255

DelayManager delayer;
Backstore store;
TempControl tempControl;
PT100 pt100;
TempAndHum tempAndHum;
RelayController relayController;
GUI gui;

bool needReload = false;

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
  store.begin();
  pt100.begin();
  tempAndHum.begin();
  relayController.begin(&tempControl);
  gui.begin(&needReload, &store, TempSensors, HumSensors);

  TempSensors[0] = &tempAndHum.temperature;
  TempSensors[1] = &pt100.temperature;
  HumSensors[0] = &tempAndHum.humidity;

  tempControl.begin(TempSensors, &store); //inicialise should be earlier than this
  //maybe there's an error due to not reading config
}

void loop()
{
  Serial.print("\n");
  checkReload();
  
  unsigned int deltatime = delayer.getDeltaTime();
  pt100.refresh();
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

  gui.endrefresh();
  delayer.sleepReamingOf(50);
}

void checkReload()
{
  if (!needReload) { return; }

  tempControl.readConfig();
}
