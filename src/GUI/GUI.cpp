#include <Arduino.h>
#include "GUI.h"

#define STATE_CHANNEL_SETTINGS 0
#define STATE_SETTINGS 1
#define STATE_NOCOMMAND 2
#define STATE_TEMP2 3

#define MAIN_SENSOR 0
#define PRESS_PERCENT 0.2

GUI::GUI() { }

void GUI::begin(float *_TempSensors[], float *_HumSensors[])
{
  oled.begin();
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
    //oled.drawSquare(122, (32-(32/6*4))*line/6, 5, (32*4/6));
    oled.drawText(STATE_CHANNEL_SETTINGS, 0, 0, "  Channel Settings  .", 1);

    oled.drawText(STATE_CHANNEL_SETTINGS, 6, 6, "Channel", 1);
    oled.drawText(STATE_CHANNEL_SETTINGS, 6, 12, "Mode", 1);
    oled.drawText(STATE_CHANNEL_SETTINGS, 6, 18, "Sensor ID", 1);
    oled.drawText(STATE_CHANNEL_SETTINGS, 6, 24, "Off Level", 1);
    oled.drawText(STATE_CHANNEL_SETTINGS, 6, 32, "On Level", 1);

    if (line != 0) { oled.drawText(STATE_CHANNEL_SETTINGS, 0, line*6, "-", 1); }

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
      if (line > 4) { line = 4; }
      else if (line < 0) { line = 0; }
      
      getJoyStick(deltatime, &pageVar, true);
      if (line == 0 && pageVar < 0) { pageVar = 0; }
      else if (line == 0 && pageVar > 0) { setState(STATE_NOCOMMAND); }
      else if (line == 1 && pageVar < 0) { setState(STATE_CHANNEL_SETTINGS); }
    }
  }


  /*
  if (oled.getCurPage() == STATE_PAGE0 || nextPage == STATE_PAGE0 || oled.getTargetPage() == STATE_PAGE0)
  {
    oled.drawText(STATE_PAGE0, 0, 0, "PAGE0 - " + numToString(analogRead(0),0), 1);

    //no switching in progress
    if (oled.getCurPage() == oled.getTargetPage())
    {
      getJoyStick(deltatime, &pageSwitcher, true);
      if (pageSwitcher < 0)
      {
        setState(STATE_TEMP2);
      }
      else if (pageSwitcher > 0)
      {
        setState(oled.getCurPage() +1);
      }
    }
  }
  if (oled.getCurPage() == STATE_PAGE1 || nextPage == STATE_PAGE1 || oled.getTargetPage() == STATE_PAGE1)
  {
    oled.drawText(STATE_PAGE1, 0, 6, "PAGE1 - " + numToString(analogRead(1),0), 1);

    //no switching in progress
    if (oled.getCurPage() == oled.getTargetPage())
    {
      getJoyStick(deltatime, &pageSwitcher, true);
      if (pageSwitcher < 0)
      {
        setState(oled.getCurPage() -1);
      }
      else if (pageSwitcher > 0)
      {
        setState(oled.getCurPage() +1);
      }
    }
  }
  if (oled.getCurPage() == STATE_NOCOMMAND || nextPage == STATE_NOCOMMAND || oled.getTargetPage() == STATE_NOCOMMAND)
  {
    oled.drawText(STATE_NOCOMMAND, 0, 12, "IDLE - " + numToString(analogRead(2),0), 1);

    //no switching in progress
    if (oled.getCurPage() == oled.getTargetPage())
    {
      getJoyStick(deltatime, &pageSwitcher, true);
      if (pageSwitcher < 0)
      {
        setState(oled.getCurPage() -1);
      }
      else if (pageSwitcher > 0)
      {
        setState(oled.getCurPage() +1);
      }
    }
  }
  if (oled.getCurPage() == STATE_TEMP2 || nextPage == STATE_TEMP2 || oled.getTargetPage() == STATE_TEMP2)
  {
    oled.drawText(STATE_TEMP2, 0, 18, "TEMP2 - " + numToString(analogRead(3),0), 1);

    //no switching in progress
    if (oled.getCurPage() == oled.getTargetPage())
    {
      getJoyStick(deltatime, &pageSwitcher, true);
      if (pageSwitcher < 0)
      {
        setState(oled.getCurPage() -1);
      }
      else if (pageSwitcher > 0)
      {
        setState(STATE_PAGE0);
      }
    }
  }
  */
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