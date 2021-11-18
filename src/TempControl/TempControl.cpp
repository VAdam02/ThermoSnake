#define CHANNEL_COUNT 2 //max 255
#define LEVEL0_MAXDELAYTIME 255 //max 255 sec due to LEVELX_MAXDELAY_LEFT
#define LEVEL1_OFFCOOLDOWN 120 //max 255 sec due to LEVELX_COOLDOWN_LEFT
#define LEVEL1_COOLSPEED_1C_IN_SECONDS 1200 //0.1C in 120 seconds -> 1C in 1200 seconds

/*
 * EEPROM allocations:
 * SAVENAME 0 - for channel 0's mode and variables
 * SAVENAME 1 - for channel 1's mode and variables
 */
#define SAVENAME 'A'

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
}

void TempControl::refresh(unsigned int deltatime)
{
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
}

void TempControl::addHeatingTask(byte channel, byte on_time, byte maxDelay_time)
{
  //TODO if on_time inicialised with 255 than make it unlimited
  channelParams[channel][LEVELX_STATE] = 2;
  getUnsignedByteFormat(on_time, LEVELX_ONTIME_LEFT, channelParams[channel]);

  float maxDelayLeft = reverseUnsignedByteFormat(LEVELX_MAXDELAY_LEFT, channelParams[channel]);
  if (maxDelayLeft == 0 || maxDelayLeft > maxDelay_time)
  {
    maxDelayLeft = maxDelay_time;
  }
  getUnsignedByteFormat(maxDelayLeft, LEVELX_MAXDELAY_LEFT, channelParams[channel]);
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
  float onLevel = reverseByteFormat(LEVEL0_ONLEVEL, channelParams[channel]);
  float offLevel = reverseByteFormat(LEVEL0_OFFLEVEL, channelParams[channel]);

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
  float cooldownLeft = reverseUnsignedByteFormat(LEVELX_COOLDOWN_LEFT, channelParams[channel]);
  float targetLevel = reverseUnsignedByteFormat(LEVEL1_TARGETLEVEL, channelParams[channel]);
  float tolerance = reverseUnsignedByteFormat(LEVEL1_TOLERANCE, channelParams[channel]) * 0.75;

  float reaction = reverseByteFormat(LEVEL1_REACTION, channelParams[channel]);

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
      getUnsignedByteFormat(cooldownLeft, LEVELX_COOLDOWN_LEFT, channelParams[channel]);
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
      getByteFormat(curLevel, LEVEL1_SAMPLE_TEMPERATURE, channelParams[channel]);
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
      getByteFormat(curLevel, LEVEL1_SAMPLE_TEMPERATURE, channelParams[channel]);
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
    getUnsignedByteFormat(cooldownLeft, LEVELX_COOLDOWN_LEFT, channelParams[channel]);
    channelParams[channel][LEVEL1_CURRENTSTATE]++;
  }

  //data analyzing
  if (channelParams[channel][LEVEL1_CURRENTSTATE] == 3 && cooldownLeft == 0)
  {
    //DEBUG
    Serial.print("sample - ");
    Serial.print("AVG: ");

    //calculate the new avg reaction
    float sampleTemperature = reverseByteFormat(LEVEL1_SAMPLE_TEMPERATURE, channelParams[channel]);

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
    getByteFormat(reaction, LEVEL1_REACTION, channelParams[channel]);
    //TODO make less write sequance
    byte data[2];
    data[0] = channelParams[channel][LEVEL1_REACTION];
    data[1] = channelParams[channel][LEVEL1_REACTION2];
    store->writeBytes(SAVENAME, channel, 6, 7, data);

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

void TempControl::getByteFormat(float data, byte index, byte array[])
{
  array[index] = ((byte)abs(data) % 128) + (data < 0 ? 128 : 0);
  array[index+1] = (abs(data) - (int)(abs(data))) * 256;
}
float TempControl::reverseByteFormat(byte index, byte array[])
{
  float value = 0;
  value += array[index] % 128;
  value += (float)(array[index+1]) / 256;
  value *= (array[index] >= 128 ? -1 : 1);
  
  return value;
}
void TempControl::getUnsignedByteFormat(float data, byte index, byte array[])
{
  array[index] = ((byte)abs(data) % 256);
  array[index+1] = (abs(data) - (int)(abs(data))) * 256;
}
float TempControl::reverseUnsignedByteFormat(byte index, byte array[])
{
  float value = 0;
  value += array[index];
  value += (float)(array[index+1]) / 256;
  
  return value;
}

void TempControl::readConfig()
{ 
  for (int i = 0; i < CHANNEL_COUNT; i++)
  {
    //read channel0's config
    byte fromStore[10];
    if (store->readBytes(SAVENAME, i, 0, 9, fromStore) != 0)
    {
      //DEBUG
      Serial.print("Inicialise\n");
      //error - not found so inicialise to null mode
      store->allocateSpace(SAVENAME, i, 10);
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
      store->writeBytes(SAVENAME, i, 0, 9, fromStore);
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