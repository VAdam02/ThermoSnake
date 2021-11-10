#define CHANNEL_COUNT 2 //max 255
#define LEVEL0_MAXDELAYTIME 255 //max 255 sec due to LEVELX_MAXDELAY_LEFT
#define LEVEL1_OFFCOOLDOWN 120 //max 255 sec due to LEVELX_COOLDOWN_LEFT
#define LEVEL1_COOLSPEED_1C_IN_SECONDS 1200 //0.1C in 120 seconds -> 1C in 1200 seconds

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


/*
 * EEPROM allocations:
 * B 0 - for channel 0's mode and variables
 * B 1 - for channel 1's mode and variables
 */
/*
 * RELAY STATES
 * 0 - Turn off
 * 1 - Turned off
 * 2 - Turn on
 * 3 - Turned on
 */

#include <Arduino.h>
#include "TempControl.h"

TempControl::TempControl() { }

void TempControl::begin(float *_sensors[], Backstore* _store)
{
    sensors = _sensors;
    store = _store;

    readConfig();

    lastTime = millis();
}

void TempControl::refresh()
{
  unsigned int deltatime = (unsigned int)(millis() % 65536) - lastTime;

  for (int i = 0; i < CHANNEL_COUNT; i++)
  {
    if (channelParams[i][LEVELX_MODE] == 0)
    {
      //not used
      //Serial.print(" LEVELX ");
    }
    else if (channelParams[i][LEVELX_MODE] == 1)
    {
      Serial.print(" LEVEL0 ");
      level0(i, *sensors[channelParams[i][LEVELX_SENSOR_ID]]);
    }
    else if (channelParams[i][LEVELX_MODE] == 2)
    {
      Serial.print(" LEVEL1 ");
      level1(i, *sensors[channelParams[i][LEVELX_SENSOR_ID]], deltatime);
    }
  }

  lastTime = millis();
}

void TempControl::addHeatingTask(byte channel, byte on_time, byte maxDelay_time)
{
  //TODO if on_time inicialised with 255 than make it unlimited
  channelParams[channel][LEVELX_STATE] = 2;
  byte data[2];
  getUnsignedByteFormat(on_time, data);
  channelParams[channel][LEVELX_ONTIME_LEFT] = data[0];
  channelParams[channel][LEVELX_ONTIME_LEFT2] = data[1];

  data[0] = channelParams[channel][LEVELX_MAXDELAY_LEFT];
  data[1] = channelParams[channel][LEVELX_MAXDELAY_LEFT2];
  float maxDelayLeft = reverseUnsignedByteFormat(data);
  if (maxDelayLeft == 0 || maxDelayLeft > maxDelay_time)
  {
    maxDelayLeft = maxDelay_time;
  }
  getUnsignedByteFormat(maxDelayLeft, data);
  channelParams[channel][LEVELX_MAXDELAY_LEFT] = data[0];
  channelParams[channel][LEVELX_MAXDELAY_LEFT2] = data[1];
}
void TempControl::stopHeatingTask(byte channel)
{
  channelParams[channel][LEVELX_STATE] = 0;
  channelParams[channel][LEVELX_ONTIME_LEFT] = 0;
  channelParams[channel][LEVELX_ONTIME_LEFT2] = 0;
  channelParams[channel][LEVELX_MAXDELAY_LEFT] = 0;
  channelParams[channel][LEVELX_MAXDELAY_LEFT2] = 0;
}

/* 
 * RETURN VALUE
 * True - new command
 * False - no new command
 */
bool TempControl::level0(byte channel, float curLevel)
{
  byte data[2];
  data[0] = channelParams[channel][LEVEL0_ONLEVEL];
  data[1] = channelParams[channel][LEVEL0_ONLEVEL2];
  float onLevel = reverseByteFormat(data);

  data[0] = channelParams[channel][LEVEL0_OFFLEVEL];
  data[1] = channelParams[channel][LEVEL0_OFFLEVEL2];
  float offLevel = reverseByteFormat(data);

  //TODO cooldown
  //TODO more intelligent maxDelay
  if (curLevel < onLevel)
  {
    //DEBUG
    Serial.print("ON - ");
    //power on
    if (channelParams[channel][LEVELX_STATE] == 0 || channelParams[channel][LEVELX_STATE] == 1)
    {
      addHeatingTask(channel, 65535, LEVEL0_MAXDELAYTIME);
      return true;
    }
    return false;
  }
  else if (offLevel < curLevel)
  {
    //DEBUG
    Serial.print("OFF - ");
    //power off
    if (channelParams[channel][LEVELX_STATE] == 2 || channelParams[channel][LEVELX_STATE] == 3)
    {
      stopHeatingTask(channel);
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
 * 3 take sample and reference with the old samples
 */
bool TempControl::level1(byte channel, float curLevel, unsigned int deltatime)
{
  byte data[2];
  data[0] = channelParams[channel][LEVELX_COOLDOWN_LEFT];
  data[1] = channelParams[channel][LEVELX_COOLDOWN_LEFT2];
  float cooldownLeft = reverseUnsignedByteFormat(data);

  data[0] = channelParams[channel][LEVEL1_TARGETLEVEL];
  data[1] = channelParams[channel][LEVEL1_TARGETLEVEL2];
  float targetLevel = reverseUnsignedByteFormat(data);

  data[0] = channelParams[channel][LEVEL1_TOLERANCE];
  data[1] = channelParams[channel][LEVEL1_TOLERANCE2];
  float tolerance = reverseUnsignedByteFormat(data) * 0.75;

  data[0] = channelParams[channel][LEVEL1_REACTION];
  data[1] = channelParams[channel][LEVEL1_REACTION2];
  float reaction = reverseByteFormat(data);

  //float reaction = 1.3; //TODO export from array

  //cooldown or heating in progress
  if (cooldownLeft > 0 || (channelParams[channel][LEVELX_ONTIME_LEFT] > 0 && channelParams[channel][LEVELX_STATE] == 3))
  {
    //decrease the left variables
    if (channelParams[channel][LEVELX_ONTIME_LEFT] > 0 && channelParams[channel][LEVELX_STATE] == 3)
    {
      //DEBUG
      Serial.print("ONwait - ");
      return false; //nothing to do
    }

    if (cooldownLeft < ((float)(deltatime)/1000))
    {
      cooldownLeft = 0;
      channelParams[channel][LEVELX_COOLDOWN_LEFT] = 0;
      channelParams[channel][LEVELX_COOLDOWN_LEFT2] = 0;
    }
    else
    {
      //DEBUG
      Serial.print("COOLwait - ");
      cooldownLeft -= ((float)(deltatime)/1000);
      getUnsignedByteFormat(cooldownLeft, data);
      channelParams[channel][LEVELX_COOLDOWN_LEFT] = data[0];
      channelParams[channel][LEVELX_COOLDOWN_LEFT2] = data[1];

      return false; //nothing to do
    }
  }

  //cooling
  if (channelParams[channel][LEVEL1_CURRENTSTATE] == 0)
  {
    //DEBUG
    Serial.print("cooling - ");
    //hot
    if (curLevel > targetLevel) { return false; } //still hot enough so nothing to do
    //cold
    //DEBUG
    Serial.print("cold - ");
    channelParams[channel][LEVEL1_CURRENTSTATE]++;
  }

  //on for x seconds
  if (channelParams[channel][LEVEL1_CURRENTSTATE] == 1 && channelParams[channel][LEVELX_STATE] != 3)
  {
    //DEBUG
    Serial.print("heat - ");
    //TODO unsigned int onTime = (targetLevel + tolerance - curLevel) / reaction * 10;
    unsigned int onTime = (targetLevel + tolerance - curLevel) / reaction * 10;
    if (onTime > channelParams[channel][LEVEL1_MAXON]) { onTime = channelParams[channel][LEVEL1_MAXON]; }

    //it could happen in some cases when the heat is become ok without activating the relay
    if (curLevel > (targetLevel + tolerance))
    {
      Serial.print("skip - ");
      channelParams[channel][LEVEL1_CURRENTSTATE] += 2;
    }
    //it's going to be cold there
    else if (curLevel < (targetLevel - tolerance))
    {
      //DEBUG
      Serial.print("SOS: ");
      Serial.print(onTime);
      Serial.print(" - ");
      addHeatingTask(channel, onTime, 0);

      //take sample
      getByteFormat(curLevel, data);
      channelParams[channel][LEVEL1_SAMPLE_TEMPERATURE] = data[0];
      channelParams[channel][LEVEL1_SAMPLE_TEMPERATURE2] = data[1];
      channelParams[channel][LEVEL1_SAMPLE_POWERONTIME] = channelParams[channel][LEVELX_ONTIME_LEFT];
    }
    //it's going to be cold there in a closed time
    else if (onTime > channelParams[channel][LEVEL1_MINON])
    {
      //DEBUG
      Serial.print("OK: ");
      Serial.print(onTime);
      Serial.print(" - ");

      if ((curLevel - (targetLevel - tolerance))*LEVEL1_COOLSPEED_1C_IN_SECONDS > 255)
      {
        addHeatingTask(channel, onTime, 255);
      }
      else
      {
        addHeatingTask(channel, onTime, (curLevel - (targetLevel - tolerance))*LEVEL1_COOLSPEED_1C_IN_SECONDS);
      }
      
      //take sample
      getByteFormat(curLevel, data);
      channelParams[channel][LEVEL1_SAMPLE_TEMPERATURE] = data[0];
      channelParams[channel][LEVEL1_SAMPLE_TEMPERATURE2] = data[1];
      channelParams[channel][LEVEL1_SAMPLE_POWERONTIME] = channelParams[channel][LEVELX_ONTIME_LEFT];
    }
  }
  else if (channelParams[channel][LEVEL1_CURRENTSTATE] == 1)
  {
    channelParams[channel][LEVEL1_CURRENTSTATE]++;
  }

  //heating in progress

  //heating finished
  if (channelParams[channel][LEVEL1_CURRENTSTATE] == 2 && channelParams[channel][LEVELX_ONTIME_LEFT] == 0 && channelParams[channel][LEVELX_STATE] == 1)
  {
    cooldownLeft = LEVEL1_OFFCOOLDOWN;
    getUnsignedByteFormat(cooldownLeft, data);
    channelParams[channel][LEVELX_COOLDOWN_LEFT] = data[0];
    channelParams[channel][LEVELX_COOLDOWN_LEFT2] = data[1];

    channelParams[channel][LEVEL1_CURRENTSTATE]++;
  }

  //data analyzing
  if (channelParams[channel][LEVEL1_CURRENTSTATE] == 3 && cooldownLeft == 0)
  {
    //DEBUG
    Serial.print("sample - ");
    Serial.print("AVG: ");

    //calculate the new avg reaction
    data[0] = channelParams[channel][LEVEL1_SAMPLE_TEMPERATURE];
    data[1] = channelParams[channel][LEVEL1_SAMPLE_TEMPERATURE2];
    float sampleTemperature = reverseByteFormat(data);

    //TODO make better avg calculation
    float curReaction = (curLevel - sampleTemperature) / (channelParams[channel][LEVEL1_SAMPLE_POWERONTIME] / 10);
    if (0 < curReaction && curReaction < 128)
    {
      reaction = (reaction * 0.5) + (curReaction * 0.5);
    }
    else
    {
      if (curReaction < 0)
      {
        reaction *= 0.75;
      }
      else
      {
        //bigger than 128 but it not really can happen
      }
      //DEBUG
      Serial.print(" ERROR_REACTION ");
    }
    getByteFormat(reaction, data);
    channelParams[channel][LEVEL1_REACTION] = data[0];
    channelParams[channel][LEVEL1_REACTION2] = data[1];
    //TODO make less write sequance
    store->writeBytes('B', channel, 6, 7, data);

    //DEBUG
    Serial.print(reaction, DEC);
    Serial.print(" - ");

    channelParams[channel][LEVEL1_CURRENTSTATE]++;
  }

  if (channelParams[channel][LEVEL1_CURRENTSTATE] >= 4)
  {
    channelParams[channel][LEVEL1_CURRENTSTATE] = 0;
    return false;
  }

  return true;
}

void TempControl::getByteFormat(float data, byte returnValue[])
{
  unsigned int fullPart = (unsigned int)abs(data);
  float otherPart = abs(data) - fullPart;

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
  float current2 = 1;
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
float TempControl::reverseByteFormat(byte data[])
{
  float value = 0;
  byte current = data[0];
  
  if (current >= 128) { current -= 128; }
  value = current;

  current = data[1];
  float otherPart = 0.5;
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
void TempControl::getUnsignedByteFormat(float data, byte returnValue[])
{
  unsigned int fullPart = (unsigned int)abs(data);
  float otherPart = abs(data) - fullPart;

  fullPart = fullPart % 256;
  unsigned int current = 256;
  byte cache = 0;
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
  float current2 = 1;
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
float TempControl::reverseUnsignedByteFormat(byte data[])
{
  float value = 0;
  byte current = data[0];
  
  value += current;

  current = data[1];
  float otherPart = 0.5;
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

  return value;
}

void TempControl::readConfig()
{ 
  for (int i = 0; i < CHANNEL_COUNT; i++)
  {
    //read channel0's config
    byte fromStore[10];
    if (store->readBytes('B', i, 0, 9, fromStore) != 0)
    {
      //DEBUG
      Serial.print("Inicialise\n");
      //error - not found so inicialise to null mode
      store->allocateSpace('B', i, 10);
      fromStore[0] = 0; //LEVELX_MODE
      fromStore[1] = 0; //LEVELX_SENSOR_ID
      fromStore[2] = 0; //LEVEL0_OFFLEVEL   LEVEL1_TARGETLEVEL
      fromStore[3] = 0; //LEVEL0_OFFLEVEL2  LEVEL1_TARGETLEVEL2
      fromStore[4] = 0; //LEVEL0_ONLEVEL    LEVEL1_TOLERANCE
      fromStore[5] = 0; //LEVEL0_ONLEVEL2   LEVEL1_TOLERANCE2
      fromStore[6] = 0; //-                 LEVEL1_REACTION
      fromStore[7] = 0; //-                 LEVEL1_REACTION2
      fromStore[8] = 0; //-                 LEVEL1_MINON
      fromStore[9] = 0; //-                 LEVEL1_MAXON
      store->writeBytes('B', i, 0, 9, fromStore);
    }
    
    channelParams[i][LEVELX_STATE] = 0;
    channelParams[i][LEVELX_MODE] = fromStore[0];
    channelParams[i][LEVELX_SENSOR_ID] = fromStore[1];
    channelParams[i][LEVELX_ONTIME_LEFT] = 0;
    channelParams[i][LEVELX_ONTIME_LEFT2] = 0;
    channelParams[i][LEVELX_COOLDOWN_LEFT] = 0;
    channelParams[i][LEVELX_COOLDOWN_LEFT2] = 0;
    channelParams[i][LEVELX_MAXDELAY_LEFT] = 0;
    channelParams[i][LEVELX_MAXDELAY_LEFT2] = 0;
    channelParams[i][9] = fromStore[2];
    channelParams[i][10] = fromStore[3];
    channelParams[i][11] = fromStore[4];
    channelParams[i][12] = fromStore[5];
    channelParams[i][13] = fromStore[6];
    channelParams[i][14] = fromStore[7];
    channelParams[i][15] = fromStore[8];
    channelParams[i][16] = fromStore[9];
    channelParams[i][17] = 0;
    channelParams[i][18] = 0;
    channelParams[i][19] = 0;
    channelParams[i][20] = 0;

    //DEBUG
    Serial.print(channelParams[i][LEVELX_MODE]);
  }
}