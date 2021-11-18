#include "src/DelayManager/DelayManager.h"
#include "src/TempAndHum/TempAndHum.h"
#include "src/Backstore/Backstore.h"
#include "src/TempControl/TempControl.h"
#include "src/RelayController/RelayController.h"

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
TempAndHum tempAndHum;
Backstore store;
TempControl tempControl;
RelayController relayController;

char charss[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y'};

float *TempSensors[TEMPSENSORCOUNT];
float *HumSensors[HUMSENSORCOUNT];

void setup()
{
  delayer.begin();
  tempAndHum.begin();
  store.begin();
  relayController.begin(&tempControl);

  Serial.begin(9600);

  TempSensors[0] = &tempAndHum.temperature;
  HumSensors[0] = &tempAndHum.humidity;

  tempControl.begin(TempSensors, &store); //inicialise should be earlier than this

  lastTime = millis();
}

void loop()
{
  //DEBUG
  Serial.print("\n");
  
  tempAndHum.refresh();
  tempControl.refresh();
  relayController.refresh();

  //CODE GOES HERE

  delayer.sleepReamingOf(50);
}

long exponentiation(long base, long exponent)
{
  long value = 1;
  for (int i = 0; i < exponent; i++)
  {
    value *= base;
  }
  return value;
}

String numToString(double num, int decimals)
{
  String data = "";
  int greater = num;
  num -= greater;
  if (greater < 1) { data = "0"; }
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
