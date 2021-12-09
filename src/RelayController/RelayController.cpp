#include "RelayController.h"

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

RelayController::RelayController() { }

void RelayController::begin(Backstore *_store, TempControl* _tempControl)
{
  store = _store;
  tempControl = _tempControl;
}

void RelayController::refresh(unsigned int deltatime)
{
  byte data[2];

  for (int i = 0; i < CHANNEL_COUNT; i++)
  {
    //maxDelayLeft
    float maxDelayLeft = tempControl->store->reverseUnsignedByteFormat(LEVELX_MAXDELAY_LEFT, tempControl->channelParams[i]);
    if (maxDelayLeft > 0 && tempControl->channelParams[i][LEVELX_STATE] == 2)
    {
      if (maxDelayLeft < ((float)(deltatime)/1000)) { maxDelayLeft = 0; }
      else { maxDelayLeft -= ((float)(deltatime)/1000); }
    }
    tempControl->store->getUnsignedByteFormat(maxDelayLeft, LEVELX_MAXDELAY_LEFT, tempControl->channelParams[i]);
    //maxDelayLeft

    //onTimeLeft
    float onTimeLeft = tempControl->store->reverseUnsignedByteFormat(LEVELX_ONTIME_LEFT, tempControl->channelParams[i]);
    
    if (0 < onTimeLeft && tempControl->channelParams[i][LEVELX_STATE] == 3)
    {
      if (onTimeLeft < 255)
      {
        if (onTimeLeft < ((float)(deltatime)/1000)) { onTimeLeft = 0; }
        else { onTimeLeft -= ((float)(deltatime)/1000); }
      }
    }
    tempControl->store->getUnsignedByteFormat(onTimeLeft, LEVELX_ONTIME_LEFT, tempControl->channelParams[i]);
    //onTimeLeft

    //this need change
    if ((maxDelayLeft >= 0 && tempControl->channelParams[i][LEVELX_STATE] == 2) || (onTimeLeft == 0 && tempControl->channelParams[i][LEVELX_STATE] != 1))
    {
      //look for an other channel that want change
      int j = i+1;
      while (j < CHANNEL_COUNT && !(((tempControl->channelParams[j][LEVELX_MAXDELAY_LEFT] >= 0 || tempControl->channelParams[j][LEVELX_MAXDELAY_LEFT2] >= 0) && tempControl->channelParams[j][LEVELX_STATE] == 2) || ((tempControl->channelParams[j][LEVELX_ONTIME_LEFT] == 0) && tempControl->channelParams[j][LEVELX_STATE] != 1))) { j++; }
      if (j < CHANNEL_COUNT)
      {
        //found
        //DEBUG
        Serial.print(" _ COMBO _ ");
        combo++;

        activate(i);
        activate(j);
      }
      else
      {
        //this need change only
        if (maxDelayLeft == 0)
        {
          activate(i);
        }
      }
    }
  }
}

void RelayController::activate(byte channel)
{
  //DEBUG
  if (channel == 0)
  {
    ch0++;
  }
  else
  {
    ch1++;
  }

  byte data[2];

  //maxDelayLeft
  float maxDelayLeft = tempControl->store->reverseUnsignedByteFormat(LEVELX_MAXDELAY_LEFT, tempControl->channelParams[channel]);
  float onTimeLeft = tempControl->store->reverseUnsignedByteFormat(LEVELX_ONTIME_LEFT, tempControl->channelParams[channel]);

  //on
  if (maxDelayLeft >= 0 && tempControl->channelParams[channel][LEVELX_STATE] == 2)
  {
    digitalWrite(channel+3, HIGH);
    tempControl->channelParams[channel][LEVELX_STATE] = 3;
    
    tempControl->store->getUnsignedByteFormat(0, LEVELX_MAXDELAY_LEFT, tempControl->channelParams[channel]);
  }
  //off
  else
  {
    if (onTimeLeft < 1 && tempControl->channelParams[channel][LEVELX_STATE] == 3)
    {
      tempControl->store->getUnsignedByteFormat(0, LEVELX_ONTIME_LEFT, tempControl->channelParams[channel]);
      tempControl->channelParams[channel][LEVELX_STATE] = 0;
    }
    if (tempControl->channelParams[channel][LEVELX_STATE] == 0)
    {
      tempControl->channelParams[channel][LEVELX_STATE] = 1;
      digitalWrite(channel+3, LOW);
    }
  }
}