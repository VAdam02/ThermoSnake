#include <Arduino.h>
#include "GUI.h"

#define STATE_CHANNEL_SETTINGS 0
#define STATE_SETTINGS 1
#define STATE_NOCOMMAND 2
#define STATE_TEMP2 3

#define MAIN_SENSOR 0
#define PRESS_PERCENT 0.2

GUI::GUI() { }

void GUI::begin(Backstore *_store, float *_TempSensors[], float *_HumSensors[])
{
  oled.begin();
  store = _store;
  TempSensors = _TempSensors;
  HumSensors = _HumSensors;
  oled.setPage(STATE_NOCOMMAND);
}

void GUI::refresh(unsigned int deltatime)
{
  timeCounter += deltatime;

  oled.clear();
  oled.refresh();
  
  if (abs(analogRead(A0) - 512) >= PRESS_PERCENT * 512)
  {
    if (presstime[0] > 65535-deltatime) { presstime[0] = 65535; }
    else { presstime[0] += deltatime; }
  } else { presstime[0] = 0; }
  if (abs(analogRead(A1) - 512) >= PRESS_PERCENT * 512)
  {
    if (presstime[1] > 65535-deltatime) { presstime[1] = 65535; }
    else { presstime[1] += deltatime; }
  } else { presstime[1] = 0; }
  
  byte nextPage = oled.getCurPage() + (oled.getCurPage() < oled.getTargetPage() ? 1 : (oled.getCurPage() == oled.getTargetPage() ? 0 : -1));

  if (oled.getCurPage() == STATE_NOCOMMAND || nextPage == STATE_NOCOMMAND || oled.getTargetPage() == STATE_NOCOMMAND)
  {
    oled.drawText(STATE_NOCOMMAND, 4, 7, numToString(*TempSensors[MAIN_SENSOR],1) + "*C", 4);

    //no switching in progress
    if (oled.getCurPage() == oled.getTargetPage())
    {
      getJoyStick(deltatime, &pageVar, true);
      if (pageVar < 0) { setState(STATE_SETTINGS); }
      else if (pageVar > 0) { pageVar = 0; }
    }
  }
  if (oled.getCurPage() == STATE_SETTINGS || nextPage == STATE_SETTINGS || oled.getTargetPage() == STATE_SETTINGS)
  {
    oled.drawText(STATE_SETTINGS, 0, 0, "      Settings      .", 1);

    oled.drawText(STATE_SETTINGS, 6, 6, "Channel Settings", 1);

    if (line != 0) { oled.drawText(STATE_SETTINGS, 0, line*6, "-", 1); }

    //no switching in progress
    if (oled.getCurPage() == oled.getTargetPage())
    {
      if (presstime[1] == 0 && lineVar != 0)
      {
        if (lineVar > 0) { line++; }
        else { line--; }
        lineVar = 0;
      }

      Serial.print(lineVar);
      getJoyStick(deltatime, &lineVar, false);
      if (line > 4) { line = 2; }
      else if (line < 0) { line = 0; }
      
      getJoyStick(deltatime, &pageVar, true);
      if (line == 0 && pageVar < 0) { pageVar = 0; }
      else if (line == 0 && pageVar > 0) { setState(STATE_NOCOMMAND); }
      else if (line == 1 && pageVar < 0) { setState(STATE_CHANNEL_SETTINGS); }
    }
  }
  if (oled.getCurPage() == STATE_CHANNEL_SETTINGS || nextPage == STATE_CHANNEL_SETTINGS || oled.getTargetPage() == STATE_CHANNEL_SETTINGS)
  {
    byte fsize = 1;
    byte lineCount = 10;
    float asd = (float)(lineCount-(32/fsize/6))/(float)(lineCount-1);
    Serial.print(asd);
    oled.drawText(STATE_CHANNEL_SETTINGS, 0, 0-(int)(line*asd*fsize*6), "  Channel Settings  .", fsize);

    oled.drawText(STATE_CHANNEL_SETTINGS, 6, 6-(int)(line*asd*fsize*6), "Channel", fsize);
    oled.drawText(STATE_CHANNEL_SETTINGS, 6, 12-(int)(line*asd*fsize*6), "Mode", fsize);
    oled.drawText(STATE_CHANNEL_SETTINGS, 6, 18-(int)(line*asd*fsize*6), "Sensor ID", fsize);
    oled.drawText(STATE_CHANNEL_SETTINGS, 6, 24-(int)(line*asd*fsize*6), "Off Level", fsize);
    oled.drawText(STATE_CHANNEL_SETTINGS, 6, 30-(int)(line*asd*fsize*6), "On Level", fsize);
    oled.drawText(STATE_CHANNEL_SETTINGS, 6, 36-(int)(line*asd*fsize*6), "6", fsize);
    oled.drawText(STATE_CHANNEL_SETTINGS, 6, 42-(int)(line*asd*fsize*6), "7", fsize);
    oled.drawText(STATE_CHANNEL_SETTINGS, 6, 48-(int)(line*asd*fsize*6), "8", fsize);
    oled.drawText(STATE_CHANNEL_SETTINGS, 6, 54-(int)(line*asd*fsize*6), "9", fsize);

    //no switching in progress
    if (oled.getCurPage() == oled.getTargetPage())
    {
      oled.drawText(STATE_CHANNEL_SETTINGS, 0, (line*fsize*6)-(int)(line*asd*fsize*6), "-", fsize);

      float sliderHeight = 32*(32/6/fsize)/lineCount;
      oled.drawSquare(122, (32-sliderHeight)*line/(lineCount-1), 5, sliderHeight);

      if (presstime[1] == 0 && lineVar != 0)
      {
        if (lineVar > 0) { line++; }
        else { line--; }
        lineVar = 0;
      }

      //Serial.print(lineVar);
      getJoyStick(deltatime, &lineVar, false);
      if (line == 255) { line = lineCount-1; }
      else if (line >= lineCount ) { line = 0; }
      
      getJoyStick(deltatime, &pageVar, true);
      if (line == 0 && pageVar < 0) { pageVar = 0; }
      else if (line == 0 && pageVar > 0) { setState(STATE_SETTINGS); }
      //else if (line == 1 && pageVar < 0) { setState(STATE_CHANNEL_SETTINGS); }
    }
  }


}

void GUI::endrefresh()
{
  oled.show();
}

void GUI::getJoyStick(unsigned int deltatime, float *data, bool horizontal)
{
  float value = 0;
  float press = 0;
  float delta = deltatime;
  if (horizontal)
  {
    value = analogRead(A0);
    press = presstime[0];
  }
  else
  {
    value = 1023-analogRead(A1);
    press = presstime[1];
  }

  //not pressed enough
  if (abs(value - 512) / 512 < PRESS_PERCENT) { return; }

  if (value - 512 > 0)
  {
    *data += ((press/1000) * (delta/1000) / 2);
  }
  else
  {
    *data -= ((press/1000) * (delta/1000) / 2);
  }
}

void GUI::setState(byte newState)
{
  if (oled.getTargetPage() == newState || oled.getCurPage() == newState) { return; }

  presstime[0] = 0;
  presstime[1] = 0;
  pageVar = 0;
  lineVar = 0;
  line = 0;
  oled.setPage(newState);

  timeCounter = 0;
}

long GUI::exponentiation(long base, long exponent)
{
  long value = 1;
  for (int i = 0; i < exponent; i++)
  {
    value *= base;
  }
  return value;
}

String GUI::numToString(double num, int decimals)
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