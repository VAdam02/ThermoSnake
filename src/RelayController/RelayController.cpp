#include <Arduino.h>
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

void RelayController::begin(TempControl* _tempControl)
{
  lastTime = millis();
  tempControl = _tempControl;
}

void RelayController::refresh()
{
  unsigned int deltatime = (unsigned int)(millis() % 65536) - lastTime;

  byte data[2];

  for (int i = 0; i < CHANNEL_COUNT; i++)
  {
    //maxDelayLeft
    data[0] = tempControl->channelParams[i][LEVELX_MAXDELAY_LEFT];
    data[1] = tempControl->channelParams[i][LEVELX_MAXDELAY_LEFT2];
    float maxDelayLeft = tempControl->reverseUnsignedByteFormat(data);
    if (maxDelayLeft > 0 && tempControl->channelParams[i][LEVELX_STATE] == 2)
    {
      if (maxDelayLeft < ((float)(deltatime)/1000)) { maxDelayLeft = 0; }
      else { maxDelayLeft -= ((float)(deltatime)/1000); }
    }
    tempControl->getUnsignedByteFormat(maxDelayLeft, data);
    tempControl->channelParams[i][LEVELX_MAXDELAY_LEFT] = data[0];
    tempControl->channelParams[i][LEVELX_MAXDELAY_LEFT2] = data[1];
    //maxDelayLeft

    //onTimeLeft
    data[0] = tempControl->channelParams[i][LEVELX_ONTIME_LEFT];
    data[1] = tempControl->channelParams[i][LEVELX_ONTIME_LEFT2];
    float onTimeLeft = tempControl->reverseUnsignedByteFormat(data);
    
    if (0 < onTimeLeft && tempControl->channelParams[i][LEVELX_STATE] == 3)
    {
      if (onTimeLeft < 255)
      {
        if (onTimeLeft < ((float)(deltatime)/1000)) { onTimeLeft = 0; }
        else { onTimeLeft -= ((float)(deltatime)/1000); }
      }
    }
    tempControl->getUnsignedByteFormat(onTimeLeft, data);
    tempControl->channelParams[i][LEVELX_ONTIME_LEFT] = data[0];
    tempControl->channelParams[i][LEVELX_ONTIME_LEFT2] = data[1];
    //onTimeLeft

    //this need change
    if ((maxDelayLeft >= 0 && tempControl->channelParams[i][LEVELX_STATE] == 2) || (onTimeLeft == 0 && tempControl->channelParams[i][LEVELX_STATE] != 1))
    {
      //look for an other channel that want change
      int j = i+1;
      while (j < CHANNEL_COUNT && !(((tempControl->channelParams[j][LEVELX_MAXDELAY_LEFT] >= 0 || tempControl->channelParams[j][LEVELX_MAXDELAY_LEFT2] >= 0) && tempControl->channelParams[j][LEVELX_STATE] == 2) || ((tempControl->channelParams[j][LEVELX_ONTIME_LEFT] == 0 || tempControl->channelParams[j][LEVELX_ONTIME_LEFT2] == 0) && tempControl->channelParams[j][LEVELX_STATE] != 1))) { j++; }
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

  lastTime = millis();
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
  data[0] = tempControl->channelParams[channel][LEVELX_MAXDELAY_LEFT];
  data[1] = tempControl->channelParams[channel][LEVELX_MAXDELAY_LEFT2];
  float maxDelayLeft = tempControl->reverseUnsignedByteFormat(data);

  data[0] = tempControl->channelParams[channel][LEVELX_ONTIME_LEFT];
  data[1] = tempControl->channelParams[channel][LEVELX_ONTIME_LEFT2];
  float onTimeLeft = tempControl->reverseUnsignedByteFormat(data);

  //on
  if (maxDelayLeft >= 0 && tempControl->channelParams[channel][LEVELX_STATE] == 2)
  {
    digitalWrite(channel+2, HIGH);
    tempControl->channelParams[channel][LEVELX_STATE] = 3;
    
    tempControl->getUnsignedByteFormat(0, data);
    tempControl->channelParams[channel][LEVELX_MAXDELAY_LEFT] = data[0];
    tempControl->channelParams[channel][LEVELX_MAXDELAY_LEFT2] = data[1];
  }
  //off
  else
  {
    if (onTimeLeft == 0 && tempControl->channelParams[channel][LEVELX_STATE] == 3)
    {
      tempControl->channelParams[channel][LEVELX_STATE] = 0;
    }
    if (tempControl->channelParams[channel][LEVELX_STATE] == 0)
    {
      tempControl->channelParams[channel][LEVELX_STATE] = 1;
      digitalWrite(channel+2, LOW);
    }
  }
}