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

float *TempSensors[TEMPSENSORCOUNT];
float *HumSensors[HUMSENSORCOUNT];

unsigned int lastTime = 0;

void setup()
{
  Serial.begin(9600);
  delayer.begin();
  store.begin();
  tempAndHum.begin();
  relayController.begin(&tempControl);
  gui.begin(TempSensors);

  TempSensors[0] = &tempAndHum.temperature;
  HumSensors[0] = &tempAndHum.humidity;


  tempControl.begin(TempSensors, &store); //inicialise should be earlier than this
  //maybe there's an error due to not reading config
}

void loop()
{
  Serial.print("\n");
  unsigned int deltatime = delayer.getDeltaTime();
  tempAndHum.refresh();
  tempControl.refresh(deltatime);
  relayController.refresh(deltatime);
  gui.refresh(deltatime);

  //CODE GOES HERE

  gui.endrefresh();
  delayer.sleepReamingOf(50);
}
