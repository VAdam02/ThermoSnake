#include "src/DelayManager/DelayManager.h"
#include "src/Graphics/Graphics.h"
#include "src/Backstore/Backstore.h"
#include "src/TempAndHum/TempAndHum.h"
#include "src/TempControl/TempControl.h"
#include "src/RelayController/RelayController.h"

#define TEMPSENSORCOUNT 1
#define HUMSENSORCOUNT 1

#define CHANNEL_COUNT 2 //max 255

DelayManager delayer;
Graphics oled;
Backstore store;
TempControl tempControl;
TempAndHum tempAndHum;
RelayController relayController;

float *TempSensors[TEMPSENSORCOUNT];
float *HumSensors[HUMSENSORCOUNT];

unsigned int lastTime = 0;

void setup()
{
  Serial.begin(9600);
  delayer.begin();
  oled.begin();
  store.begin();
  tempAndHum.begin();
  relayController.begin(&tempControl);

  TempSensors[0] = &tempAndHum.temperature;
  HumSensors[0] = &tempAndHum.humidity;


  tempControl.begin(TempSensors, &store); //inicialise should be earlier than this
  //maybe there's an error due to not reading config

  store.mem();
}

void loop()
{
  oled.clear();
  oled.refresh();
  tempAndHum.refresh();
  tempControl.refresh();
  relayController.refresh();

  unsigned int deltatime = (unsigned int)(millis() % 65536) - lastTime;
  lastTime = millis();

  //CODE GOES HERE

  oled.show();
  delayer.sleepReamingOf(50);
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
