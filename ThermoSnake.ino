#include "src/DelayManager/DelayManager.h"
#include "src/Backstore/Backstore.h"
#include "src/TempAndHum/TempAndHum.h"
#include "src/TempControl/TempControl.h"
#include "src/RelayController/RelayController.h"
#include "src/GUI/GUI.h"

#define TEMPSENSORCOUNT 1
#define HUMSENSORCOUNT 1

#define CHANNEL_COUNT 2 //max 255

DelayManager delayer;
Backstore store;
TempControl tempControl;
TempAndHum tempAndHum;
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

  tempControl.readConfig();
}
