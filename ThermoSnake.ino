#include "src/DelayManager/DelayManager.h"
#include "src/Backstore/Backstore.h"
#include "src/PT100/PT100.h"
#include "src/TempAndHum/TempAndHum.h"
#include "src/TempControl/TempControl.h"
#include "src/RelayController/RelayController.h"
#include "src/GUI/GUI.h"
#include "src/Buzzer/Buzzer.h"

#define TEMPSENSORCOUNT 4
#define HUMSENSORCOUNT 1

#define CHANNEL_COUNT 2 //max 255

DelayManager delayer;
Backstore store;
TempControl tempControl;
PT100 pt100;
TempAndHum tempAndHum;
RelayController relayController;
GUI gui;
Buzzer buzzer;

bool needReload = false;

float *TempSensors[TEMPSENSORCOUNT];
float *HumSensors[HUMSENSORCOUNT];

unsigned int lastTime = 0;

void setup()
{
  Serial.begin(9600);
  delayer.begin();
  store.begin();
  pt100.begin();
  tempAndHum.begin();
  relayController.begin(&tempControl);
  buzzer.begin(9);
  gui.begin(&needReload, &store, TempSensors, HumSensors);

  TempSensors[1] = &pt100.temperature;
  TempSensors[2] = &value0;
  TempSensors[3] = &value1;
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
  buzzer.refresh();

  //CODE GOES HERE

  gui.endrefresh();
  delayer.sleepReamingOf(50);
}

void checkReload()
{
  if (!needReload) { return; }

  tempControl.readConfig();
}
