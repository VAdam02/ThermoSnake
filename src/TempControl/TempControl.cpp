#define chanel_COUNT 2
#define LEVEL0_MAXDELAYTIME 120
#define LEVEL1_OFFCOOLDOWN 3000
#define LEVEL1_COOLSPEED_1C_IN_SECONDS 1200

//GENERAL
#define LEVELX_STATE 0
#define LEVELX_MODE 1
#define LEVELX_ONTIME_LEFT 2
#define LEVELX_COOLDOWN_LEFT 3
#define LEVELX_COOLDOWN_LEFT2 4
#define LEVELX_MAXDELAY_LEFT 5

//LEVEL 0
#define LEVEL0_OFFLEVEL 6
#define LEVEL0_OFFLEVEL2 7
#define LEVEL0_ONLEVEL 8
#define LEVEL0_ONLEVEL2 9

//LEVEL 1
#define LEVEL1_TARGETLEVEL 6
#define LEVEL1_TARGETLEVEL2 7
#define LEVEL1_DIFFERENCE 8
#define LEVEL1_DIFFERENCE2 9
#define LEVEL1_REACTION 10
#define LEVEL1_REACTION2 11
#define LEVEL1_MINON 12
#define LEVEL1_MAXON 13
#define LEVEL1_CURRENTSTATE 14
#define LEVEL1_SAMPLE_TEMPERATURE 15
#define LEVEL1_SAMPLE_TEMPERATURE2 16
#define LEVEL1_SAMPLE_POWERONTIME 17


/*
 * EEPROM allocations:
 * B 0 - for chanel 0's mode and variables
 * B 1 - for chanel 1's mode and variables
 */
/*
 * RELAY STATES
 * 0 - Turn off
 * 1 - Turned off
 * 2 - Turn on
 * 3 - Turned on
 */

#include "TempControl.h"
#include <Arduino.h>

TempControl::TempControl() { }

void TempControl::begin(TempAndHum* _sensor0, Backstore* _store)
{
    sensor0 = _sensor0;
    store = _store;

    readConfig();
}

void TempControl::refresh()
{
  unsigned int deltatime = (unsigned int)(millis() % 65536) - lastTime;

  
}

void TempControl::addHeatingTask(byte chanel, unsigned int on_time, unsigned int maxDelay_time)
{
  //TODO if on_time inicialised with 65535 than make it unlimited
  chanelParams[chanel][0] = 2;
  chanelParams[chanel][LEVELX_ONTIME_LEFT] = on_time;

  if (chanelParams[chanel][LEVELX_MAXDELAY_LEFT] == 0 || chanelParams[chanel][LEVELX_MAXDELAY_LEFT] > maxDelay_time)
  {
    chanelParams[chanel][LEVELX_MAXDELAY_LEFT] = maxDelay_time;
  }
}
void TempControl::stopHeatingTask(byte chanel)
{
  chanelParams[chanel][0] = 0;
  chanelParams[chanel][LEVELX_ONTIME_LEFT] = 0;
  chanelParams[chanel][LEVELX_MAXDELAY_LEFT] = 0;
}

/* 
 * RETURN VALUE
 * True - new command
 * False - no new command
 */
bool TempControl::level0(byte chanel, double curLevel)
{
  byte data[2];
  data[0] = chanelParams[chanel][LEVEL0_ONLEVEL];
  data[1] = chanelParams[chanel][LEVEL0_ONLEVEL2];
  double onLevel = reverseByteFormat(data);

  data[0] = chanelParams[chanel][LEVEL0_OFFLEVEL];
  data[1] = chanelParams[chanel][LEVEL0_OFFLEVEL2];
  double offLevel = reverseByteFormat(data);

  if (curLevel < onLevel)
  {
    //power on
    if (chanelParams[chanel][LEVELX_STATE] == 0 || chanelParams[chanel][LEVELX_STATE] == 1)
    {
      addHeatingTask(chanel, 65535, LEVEL0_MAXDELAYTIME);
      return true;
    }
    return false;
  }
  else if (offLevel < curLevel)
  {
    //power off
    if (chanelParams[chanel][LEVELX_STATE] == 2 || chanelParams[chanel][LEVELX_STATE] == 3)
    {
      stopHeatingTask(chanel);
      return true;
    }
    return false;
  }
}

/*
 * STATES
 * 0 cooling
 * 1 on for x seconds and off
 * 1.5 start heating and take sample
 * 2 heating
 * 2.5 take sample and reference with the old samples
 */
bool TempControl::level1(byte chanel, double curLevel, unsigned int deltatime)
{
  byte data[2];
  data[0] = chanelParams[chanel][LEVELX_COOLDOWN_LEFT];
  data[1] = chanelParams[chanel][LEVELX_COOLDOWN_LEFT2];
  unsigned int cooldownLeft = (int)(data[0]) * 256 + data[1];

  data[0] = chanelParams[chanel][LEVEL1_TARGETLEVEL];
  data[1] = chanelParams[chanel][LEVEL1_TARGETLEVEL2];
  double targetLevel = reverseByteFormat(data);

  data[0] = chanelParams[chanel][LEVEL1_DIFFERENCE];
  data[1] = chanelParams[chanel][LEVEL1_DIFFERENCE2];
  double difference = reverseByteFormat(data) * 0.75;

  data[0] = chanelParams[chanel][LEVEL1_REACTION];
  data[1] = chanelParams[chanel][LEVEL1_REACTION2];
  double reaction = reverseByteFormat(data);

  //double reaction = 1.3; //TODO export from array

  //cooldown or heating in progress
  if (cooldownLeft > 0 || (chanelParams[chanel][LEVELX_ONTIME_LEFT] > 0 && chanelParams[chanel][LEVELX_STATE] == 3))
  {
    //decrease the left variables
    if (chanelParams[chanel][LEVELX_ONTIME_LEFT] > 0 && chanelParams[chanel][LEVELX_STATE] == 3)
    {
      //DEBUG
      Serial.print("ONwait - ");
      return false; //nothing to do
    }

    if (cooldownLeft < deltatime)
    {
      cooldownLeft = 0;
      chanelParams[chanel][LEVELX_COOLDOWN_LEFT] = 0;
      chanelParams[chanel][LEVELX_COOLDOWN_LEFT2] = 0;
    }
    else
    {
      //DEBUG
      Serial.print("COOLwait - ");
      cooldownLeft -= deltatime;
      chanelParams[chanel][LEVELX_COOLDOWN_LEFT] = ((int)(cooldownLeft) >> 8);
      chanelParams[chanel][LEVELX_COOLDOWN_LEFT2] = ((int)(cooldownLeft) & 0xFF);

      return false; //nothing to do
    }
  }

  //cooling
  if (chanelParams[chanel][LEVEL1_CURRENTSTATE] == 0)
  {
    //DEBUG
    Serial.print("cooling - ");
    //hot
    if (curLevel > targetLevel) { return false; } //still hot enough so nothing to do
    //cold
    //DEBUG
    Serial.print("cold - ");
    chanelParams[chanel][LEVEL1_CURRENTSTATE]++;
  }

  //on for x seconds
  if (chanelParams[chanel][LEVEL1_CURRENTSTATE] == 1 && chanelParams[chanel][LEVELX_STATE] != 3)
  {
    //DEBUG
    Serial.print("heat - ");
    //TODO unsigned int onTime = (targetLevel + difference - curLevel) / reaction * 10;
    unsigned int onTime = (targetLevel + difference - curLevel) / reaction * 10;
    if (onTime > chanelParams[chanel][LEVEL1_MAXON]) { onTime = chanelParams[chanel][LEVEL1_MAXON]; }

    //it could happen in some cases when the heat is become ok without activating the relay
    if (curLevel > (targetLevel + difference))
    {
      Serial.print("skip - ");
      chanelParams[chanel][LEVEL1_CURRENTSTATE] += 2;
    }
    //it's going to be cold there
    else if (curLevel < (targetLevel - difference))
    {
      //DEBUG
      Serial.print("SOS: ");
      Serial.print(onTime);
      Serial.print(" - ");
      addHeatingTask(1, onTime, 0);

      //take sample
      getByteFormat(curLevel, data);
      chanelParams[chanel][LEVEL1_SAMPLE_TEMPERATURE] = data[0];
      chanelParams[chanel][LEVEL1_SAMPLE_TEMPERATURE2] = data[1];
      chanelParams[chanel][LEVEL1_SAMPLE_POWERONTIME] = chanelParams[chanel][LEVELX_ONTIME_LEFT];
    }
    //it's going to be cold there in a closed time
    else if (onTime > chanelParams[chanel][LEVEL1_MINON])
    {
      //DEBUG
      Serial.print("OK: ");
      Serial.print(onTime);
      Serial.print(" - ");

      addHeatingTask(1, onTime, (curLevel - (targetLevel - difference))*LEVEL1_COOLSPEED_1C_IN_SECONDS);
      //take sample
      getByteFormat(curLevel, data);
      chanelParams[chanel][LEVEL1_SAMPLE_TEMPERATURE] = data[0];
      chanelParams[chanel][LEVEL1_SAMPLE_TEMPERATURE2] = data[1];
      chanelParams[chanel][LEVEL1_SAMPLE_POWERONTIME] = chanelParams[chanel][LEVELX_ONTIME_LEFT];
    }
  }
  else if (chanelParams[chanel][LEVEL1_CURRENTSTATE] == 1)
  {
    chanelParams[chanel][LEVEL1_CURRENTSTATE]++;
  }

  //heating in progress

  //heating finished and data analyzing
  if (chanelParams[chanel][LEVEL1_CURRENTSTATE] == 2 && chanelParams[chanel][LEVELX_ONTIME_LEFT] == 0 && chanelParams[chanel][LEVELX_STATE] == 1)
  {
    //DEBUG
    Serial.print("sample - ");
    Serial.print("AVG: ");
    //calculate the new avg reaction
    data[0] = chanelParams[chanel][LEVEL1_SAMPLE_TEMPERATURE];
    data[1] = chanelParams[chanel][LEVEL1_SAMPLE_TEMPERATURE2];
    double sampleTemperature = reverseByteFormat(data);
    //TODO
    //TODO reaction = (reaction * 0.5) + ((curLevel - sampleTemperature) / (chanelParams[chanel][LEVEL1_SAMPLE_POWERONTIME] / 10) * 0.5);
    reaction = (reaction * 0.5) + ((curLevel - sampleTemperature) / (chanelParams[chanel][LEVEL1_SAMPLE_POWERONTIME] / 10) * 0.5);
    getByteFormat(reaction, data);
    chanelParams[chanel][LEVEL1_REACTION] = data[0];
    chanelParams[chanel][LEVEL1_REACTION2] = data[1];
    
    //DEBUG
    Serial.print(reaction);
    Serial.print(" - ");
    cooldownLeft = LEVEL1_OFFCOOLDOWN;
    chanelParams[chanel][LEVELX_COOLDOWN_LEFT] = ((int)(cooldownLeft) >> 8);
    chanelParams[chanel][LEVELX_COOLDOWN_LEFT2] = ((int)(cooldownLeft) & 0xFF);

    chanelParams[chanel][LEVEL1_CURRENTSTATE]++;
  }

  if (chanelParams[chanel][LEVEL1_CURRENTSTATE] >= 3)
  {
    chanelParams[chanel][LEVEL1_CURRENTSTATE] = 0;
    return false;
  }

  return true;
}

void TempControl::getByteFormat(double data, byte returnValue[])
{
  unsigned int fullPart = (unsigned int)abs(data);
  double otherPart = abs(data) - fullPart;

  fullPart = fullPart % 128;
  byte current = 128;
  byte cache = 0;
  //negative = 1 positive = 0
  if (data < 0) { cache++; }
  //full part
  while (current >= 2)
  {
    cache *= 2;
    current /= 2;

    if (fullPart >= current)
    {
      cache++;
      fullPart -= current;
    }
  }
  returnValue[0] = cache;
  cache = 0;

  //other part
  int i = 0;
  double current2 = 1;
  while (i < 8)
  {
    cache *= 2;
    current2 /= 2;

    if (otherPart >= current2)
    {
      cache++;
      otherPart -= current2;
    }

    i++;
  }
  returnValue[1] = cache;
}
double TempControl::reverseByteFormat(byte data[])
{
  double value = 0;
  byte current = data[0];
  
  if (current >= 128) { current -= 128; }
  value = current;

  current = data[1];
  double otherPart = 0.5;
  byte reference = 128;
  while (current > 0)
  {
    if (current >= reference)
    {
      current -= reference;
      value += otherPart;
    }

    otherPart /= 2;
    reference /= 2;
  }

  if (data[0] >= 128) { value *= -1; }

  return value;
}

void TempControl::readConfig()
{ 
  for (int i = 0; i < chanel_COUNT; i++)
  {
    //read chanel0's config
    byte fromStore[9];
    if (store->readBytes('B', i, 0, 8, fromStore) != 0)
    {
      //DEBUG
      Serial.print("Inicialise\n");
      //error - not found so inicialise to null mode
      store->allocateSpace('B', i, 9);
      fromStore[0] = 0; //LEVELX_MODE
      fromStore[1] = 0; //LEVEL0_OFFLEVEL   LEVEL1_TARGETLEVEL
      fromStore[2] = 0; //LEVEL0_OFFLEVEL2  LEVEL1_TARGETLEVEL2
      fromStore[3] = 0; //LEVEL0_ONLEVEL    LEVEL1_DIFFERENCE
      fromStore[4] = 0; //LEVEL0_ONLEVEL2   LEVEL1_DIFFERENCE2
      fromStore[5] = 0; //-                 LEVEL1_REACTION
      fromStore[6] = 0; //-                 LEVEL1_REACTION2
      fromStore[7] = 0; //-                 LEVEL1_MINON
      fromStore[8] = 0; //-                 LEVEL1_MAXON
      store->writeBytes('B', i, 0, 8, fromStore);
    }
    
    chanelParams[i][LEVELX_STATE] = 0;
    chanelParams[i][LEVELX_MODE] = fromStore[0];
    chanelParams[i][LEVELX_ONTIME_LEFT] = 0;
    chanelParams[i][LEVELX_COOLDOWN_LEFT] = 0;
    chanelParams[i][LEVELX_COOLDOWN_LEFT2] = 0;
    chanelParams[i][LEVELX_MAXDELAY_LEFT] = 0;
    chanelParams[i][6] = fromStore[1];
    chanelParams[i][7] = fromStore[2];
    chanelParams[i][8] = fromStore[3];
    chanelParams[i][9] = fromStore[4];
    chanelParams[i][10] = fromStore[5];
    chanelParams[i][11] = fromStore[6];
    chanelParams[i][12] = fromStore[7];
    chanelParams[i][13] = fromStore[8];
    chanelParams[i][14] = 0;
    chanelParams[i][15] = 0;
    chanelParams[i][16] = 0;
    chanelParams[i][17] = 0;

    //DEBUG
    Serial.print(chanelParams[i][LEVELX_MODE]);
  }
}