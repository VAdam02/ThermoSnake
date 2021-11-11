#include <Arduino.h>
#include "GUI.h"

#define STATE_NOCOMMAND 0
#define STATE_TEMP2 1

#define MAIN_SENSOR 0
#define PRESS_PERCENT 0.1

GUI::GUI() { }

void GUI::begin(float *_sensors[])
{
  oled.begin();
  sensors = _sensors;
}

void GUI::refresh(unsigned int deltatime)
{
  timeCounter += deltatime;

  oled.clear();
  oled.refresh();

  if (oled.getTargetPage() != nextState)
  oled.setPage(nextState);
  if (oled.getCurPage() == oled.getTargetPage())
  state = oled.getCurPage();


  
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
  



  if (state == STATE_NOCOMMAND || nextState == STATE_NOCOMMAND)
  {
    //oled.drawText(0, 4, 7, numToString(*sensors[MAIN_SENSOR],1) + "*C", 4);
    oled.drawText(0, 4, 7, numToString(pageSwitcher,2) + "*C", 4);

    //no switching in progress
    if (state == nextState)
    {
      getJoyStick(deltatime, &pageSwitcher, true);
      if (pageSwitcher > 0)
      {
        setState(1);
      }
    }
  }
  if (state == STATE_TEMP2 || nextState == STATE_TEMP2)
  {
    byte fsize = 1;
    oled.drawText(1, 0, 0, "Temp: " + numToString(2, 2) + "*C", fsize);
    oled.drawText(1, 0, 0+(fsize*6), "Avg: " + numToString(3, 2) + "*C", fsize);
    oled.drawText(1, 0, 0+2*(fsize*6), "Dif: " + numToString(4, 2), fsize);
    oled.drawText(1, 0, 0+3*(fsize*6), String(timeCounter), fsize);

    //no switching in progress
    if (state == nextState)
    {
      getJoyStick(deltatime, &pageSwitcher, true);
      if (pageSwitcher < 0)
      {
        setState(0);
      }
    }
  }

  /*
  Serial.print(deltatime);
  Serial.print(" - ");
  Serial.print(timeCounter);
  if (timeCounter < 4*exponentiation(2,12))
  {
    if (oled.getTargetPage() != 0)
    oled.setPage(0);
  }
  else if (timeCounter < 6*exponentiation(2,12))
  {
    if (oled.getTargetPage() != 1)
    oled.setPage(1);
  }
  else if (timeCounter < 8*exponentiation(2,12))
  {
    if (oled.getTargetPage() != 2)
    oled.setPage(2);
  }
  else
  {
    timeCounter -= 8*exponentiation(2,12);
  }

  oled.drawText(0, 4, 7, numToString(6,1) + "*C", 4);
  
  int x;
  int fsize = 1;

  float range = 0;
  float val = 1;
  oled.drawText(1, 0, 0, "Temp: " + numToString(2, 2) + "*C", fsize);
  oled.drawText(1, 0, 0+(fsize*6), "Avg: " + numToString(val, 2) + "*C", fsize);
  oled.drawText(1, 0, 0+2*(fsize*6), "Dif: " + numToString(3, 2), fsize);
  oled.drawText(1, 0, 0+3*(fsize*6), String(timeCounter), fsize);

  range = 4;
  val = 5;
  oled.drawText(2, 0, 0, "Hum: " + numToString(6, 2) + "%", fsize);
  oled.drawText(2, 0, 0+1*(fsize*6), "Avg: " + numToString(val, 2) + "%", fsize);
  oled.drawText(2, 0, 0+2*(fsize*6), "Dif: " + numToString(7, 2), fsize);
  oled.drawText(2, 0, 0+3*(fsize*6), String(timeCounter), fsize);
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
    value = analogRead(A1);
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
  if (nextState == newState || state == newState) { return; }

  presstime[0] = 0;
  presstime[1] = 0;
  pageSwitcher = 0;
  nextState = newState;

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