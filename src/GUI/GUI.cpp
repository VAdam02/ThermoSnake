#include "GUI.h"

#define SAVENAME 'A'
#define CHANNEL_COUNT 2
#define MODE_COUNT 3
#define TEMP_SENSOR_COUNT 2

#define STATE_CALIBRATE_SENSOR 0
#define STATE_CHANNEL_SETTINGS 1
#define STATE_SETTINGS 2
#define STATE_NOCOMMAND 3
#define STATE_TEMP2 4

#define MAIN_SENSOR 0
#define PRESS_PERCENT 0.5

#define MAX_AFK 60000

GUI::GUI() { }

void GUI::begin(bool *_needReload, Backstore *_store, float *_TempSensors[], float *_HumSensors[])
{
  oled.begin();
  needReload = _needReload;
  store = _store;
  TempSensors = _TempSensors;
  HumSensors = _HumSensors;
  oled.setPage(STATE_NOCOMMAND);
}

void GUI::refresh(unsigned int deltatime)
{
  timeCounter += deltatime;

  //AFK
  if (presstime[0] > 0 || presstime[1] > 0) { afk_time = 0; }
  else if (afk_time < MAX_AFK) { afk_time += deltatime; }
  else { setState(STATE_NOCOMMAND); }

  oled.clear();
  oled.refresh();
  
  if (abs(analogRead(A6) - 512) >= PRESS_PERCENT * 512)
  {
    if (presstime[0] > 65535-deltatime) { presstime[0] = 65535; }
    else { presstime[0] += deltatime; }
  } else { presstime[0] = 0; }
  if (abs(analogRead(A7) - 512) >= PRESS_PERCENT * 512)
  {
    if (presstime[1] > 65535-deltatime) { presstime[1] = 65535; }
    else { presstime[1] += deltatime; }
  } else { presstime[1] = 0; }
  
  byte nextPage = oled.getCurPage() + (oled.getCurPage() < oled.getTargetPage() ? 1 : (oled.getCurPage() == oled.getTargetPage() ? 0 : -1));

  if (oled.getCurPage() == STATE_TEMP2 || nextPage == STATE_TEMP2 || oled.getTargetPage() == STATE_TEMP2)
  {
    oled.drawText(STATE_TEMP2, 0, 0, F("     ALL SENSOR     ."), 1);
    oled.drawText(STATE_TEMP2, 0, 6, numToString(*TempSensors[0],1) + "*C", 1);
    oled.drawText(STATE_TEMP2, 0, 12, numToString(*HumSensors[0],1) + "%", 1);
    oled.drawText(STATE_TEMP2, 0, 18, numToString(*TempSensors[1],1) + "*C", 1);
    oled.drawText(STATE_TEMP2, 0, 24, numToString(*HumSensors[1],1) + "%", 1);

    //no switching in progress
    if (oled.getCurPage() == oled.getTargetPage())
    {
      getJoyStick(deltatime, &pageVar, true);
      if (pageVar < 0) { setState(STATE_NOCOMMAND); }
      else if (pageVar > 0) { pageVar = 0; }
    }
  }
  if (oled.getCurPage() == STATE_NOCOMMAND || nextPage == STATE_NOCOMMAND || oled.getTargetPage() == STATE_NOCOMMAND)
  {
    oled.drawText(STATE_NOCOMMAND, 4, 7, numToString(*TempSensors[MAIN_SENSOR],1) + "*C", 4);

    //no switching in progress
    if (oled.getCurPage() == oled.getTargetPage())
    {
      getJoyStick(deltatime, &pageVar, true);
      if (pageVar < 0) { setState(STATE_SETTINGS); }
      else if (pageVar > 0) { setState(STATE_TEMP2); }
    }
  }
  if (oled.getCurPage() == STATE_SETTINGS || nextPage == STATE_SETTINGS || oled.getTargetPage() == STATE_SETTINGS)
  {
    lineCount = 4;

    oled.drawText(STATE_SETTINGS, 0, 0, F("      Settings      ."), 1);

    oled.drawText(STATE_SETTINGS, 6, 6, F("Channel Settings"), 1);
    oled.drawText(STATE_SETTINGS, 6, 12, F("Calibrate Sensor"), 1);
    oled.drawText(STATE_SETTINGS, 6, 18, F("Factory reset"), 1);

    oled.drawText(STATE_SETTINGS, 0, line*6, "-", 1);

    //no switching in progress
    if (oled.getCurPage() == oled.getTargetPage())
    {
      if (presstime[1] == 0 && lineVar != 0)
      {
        if (lineVar > 0) { line++; }
        else { line--; }
        lineVar = 0;
      }

      getJoyStick(deltatime, &lineVar, false);
      if (line == 255) { line = lineCount-1; }
      else if (line >= lineCount ) { line = 0; }
      
      getJoyStick(deltatime, &pageVar, true);
      if (line == 0 && pageVar < 0) { pageVar = 0; }
      else if (line == 0 && pageVar > 0) { setState(STATE_NOCOMMAND); }
      //CHANNEL SETTINGS
      else if (line == 1 && pageVar < 0) { setState(STATE_CHANNEL_SETTINGS); }
      //CALIBRATE SENSOR
      else if (line == 2 && pageVar < 0) { setState(STATE_CALIBRATE_SENSOR); }
      //FACTORY RESET
      else if (line == 3 && pageVar < 0) { message(1,F("Factory reset")); store->inicialise(256); *needReload = true; pageVar = 0; }
    }
  }
  if (oled.getCurPage() == STATE_CHANNEL_SETTINGS || nextPage == STATE_CHANNEL_SETTINGS || oled.getTargetPage() == STATE_CHANNEL_SETTINGS)
  {
    if (nextPage != oled.getCurPage() && nextPage == STATE_CHANNEL_SETTINGS) { readConfig(channel, chSettings); }
    lineCount = (chSettings[0] == 0 ? 4 : chSettings[0] == 1 ? 7 : /*mode 2*/ 9);

    byte fsize = 1;
    float lineFit = 32/fsize/6;
    float thing = (float)(lineCount-lineFit)/(float)(lineCount-1); //percent of lines that not fit
    byte lineOffSet = (lineCount < lineFit ? 0 : line*thing*fsize*6);

    oled.drawText(STATE_CHANNEL_SETTINGS, 0, 0-lineOffSet, "  Channel Settings  .", fsize);
    oled.drawText(STATE_CHANNEL_SETTINGS, 6, 6-lineOffSet, "Channel", fsize);
    oled.drawText(STATE_CHANNEL_SETTINGS, 104, 6-lineOffSet, "(" + String(channel) + ")", fsize);
    oled.drawText(STATE_CHANNEL_SETTINGS, 6, 12-lineOffSet, "Mode", fsize);
    oled.drawText(STATE_CHANNEL_SETTINGS, 104, 12-lineOffSet, "(" + String(chSettings[0]) + ")", fsize);
    oled.drawText(STATE_CHANNEL_SETTINGS, 6, 18-lineOffSet, "Save         Revert", fsize);

    if (chSettings[0] != 0)
    {
      oled.drawText(STATE_CHANNEL_SETTINGS, 6, 24-lineOffSet, "Sensor ID", fsize);
      oled.drawText(STATE_CHANNEL_SETTINGS, 104, 24-lineOffSet, "(" + String(chSettings[1]) + ")", fsize);
    }

    if (chSettings[0] == 0) {}
    else if (chSettings[0] == 1)
    {
      oled.drawText(STATE_CHANNEL_SETTINGS, 6, 30-lineOffSet, "Off Level", fsize);
      oled.drawText(STATE_CHANNEL_SETTINGS, 89, 30-lineOffSet, "(" + String(reverseByteFormat(2, chSettings), 1) + ")", fsize);
      oled.drawText(STATE_CHANNEL_SETTINGS, 6, 36-lineOffSet, "On Level", fsize);
      oled.drawText(STATE_CHANNEL_SETTINGS, 89, 36-lineOffSet, "(" + String(reverseByteFormat(4, chSettings), 1) + ")", fsize);
    }
    else if (chSettings[0] == 2)
    {
      oled.drawText(STATE_CHANNEL_SETTINGS, 6, 30-lineOffSet, "Target Level", fsize);
      oled.drawText(STATE_CHANNEL_SETTINGS, 89, 30-lineOffSet, "(" + String(reverseByteFormat(2, chSettings), 1) + ")", fsize);
      oled.drawText(STATE_CHANNEL_SETTINGS, 6, 36-lineOffSet, "Tolerance", fsize);
      oled.drawText(STATE_CHANNEL_SETTINGS, 89, 36-lineOffSet, "(" + String(reverseUnsignedByteFormat(4, chSettings), 1) + ")", fsize);
      oled.drawText(STATE_CHANNEL_SETTINGS, 6, 42-lineOffSet, "Min On", fsize);
      oled.drawText(STATE_CHANNEL_SETTINGS, 92, 42-lineOffSet, "(" + String(chSettings[8]) + ")", fsize);
      oled.drawText(STATE_CHANNEL_SETTINGS, 6, 48-lineOffSet, "Max On", fsize);
      oled.drawText(STATE_CHANNEL_SETTINGS, 92, 48-lineOffSet, "(" + String(chSettings[9]) + ")", fsize);
    }

    //no switching in progress
    if (oled.getCurPage() == oled.getTargetPage())
    {
      oled.drawText(STATE_CHANNEL_SETTINGS, 0, (line*fsize*6)-lineOffSet, "-", fsize);

      float sliderHeight = 32*(32/6/fsize)/lineCount;
      oled.drawSquare(123, (32-sliderHeight)*line/(lineCount-1), 5, sliderHeight);

      if (presstime[1] == 0 && lineVar != 0)
      {
        if (lineVar > 0) { line++; }
        else { line--; }
        lineVar = 0;
      }

      getJoyStick(deltatime, &lineVar, false);
      if (line == 255) { line = lineCount-1; }
      else if (line >= lineCount ) { line = 0; }
      
      getJoyStick(deltatime, &pageVar, true);
      bool needRead = ((line == 1 && pageVar != 0 && presstime[0] == 0) ? true : false );

      //HEAD
      if (line == 0 && pageVar < 0) { pageVar = 0; }
      else if (line == 0 && pageVar > 0) { setState(STATE_SETTINGS); pageVar = 0; }
      //CHANNEL
      if (line == 1 && pageVar < 0 && presstime[0] == 0) { channel = (channel > 0 ? channel-1 : 0); pageVar = 0; }
      else if (line == 1 && pageVar > 0 && presstime[0] == 0) { channel = (channel < CHANNEL_COUNT-1 ? channel+1 : CHANNEL_COUNT-1); pageVar = 0; }
      //MODE
      if (line == 2 && pageVar < 0 && presstime[0] == 0) { chSettings[0] = (chSettings[0] > 0 ? chSettings[0]-1 : 0); pageVar = 0; }
      else if (line == 2 && pageVar > 0 && presstime[0] == 0) { chSettings[0] = (chSettings[0] < MODE_COUNT-1 ? chSettings[0]+1 : MODE_COUNT-1); pageVar = 0; }
      //SAVE and REVERT
      if (line == 3 && pageVar < 0) { writeConfig(channel, chSettings); message(1,F("Saved successfully")); pageVar = 0; }
      else if (line == 3 && pageVar > 0) { readConfig(channel, chSettings); message(1,F("Reverted successfully")); pageVar = 0; }
      //SENSOR
      if (line == 4 && pageVar < 0 && presstime[0] == 0) { chSettings[1] = (chSettings[1] > 0 ? chSettings[1]-1 : 0); pageVar = 0; }
      else if (line == 4 && pageVar > 0 && presstime[0] == 0) { chSettings[1] = (chSettings[1] < TEMP_SENSOR_COUNT-1 ? chSettings[1]+1 : TEMP_SENSOR_COUNT-1); pageVar = 0; }
      //NONE - OFFLEVEL - TARGETLEVEL
      if (line == 5 && pageVar != 0) { getByteFormat(reverseByteFormat(2, chSettings) + pageVar, 2, chSettings); pageVar = 0; }
      //NONE - ONLEVEL - TOLERANCE
      if (line == 6 && pageVar != 0 && chSettings[0] == 1)
      {
        getByteFormat(reverseByteFormat(4, chSettings) + pageVar, 4, chSettings);
        pageVar = 0;
      }
      else if (line == 6 && pageVar != 0 && chSettings[0] == 2)
      {
        getUnsignedByteFormat(reverseUnsignedByteFormat(4, chSettings) + pageVar, 4, chSettings);
        pageVar = 0;
      }
      //NONE - NONE - MINON
      if (line == 7 && (pageVar != 0 && presstime[0] == 0) && chSettings[0] == 2)
      {
        chSettings[8] += (pageVar < 0 ? -1 : 1);
        pageVar = 0;
      }
      //NONE - NONE - MAXON
      if (line == 8 && (pageVar != 0 && presstime[0] == 0) && chSettings[0] == 2)
      {
        chSettings[9] += (pageVar < 0 ? -1 : 1);
        pageVar = 0;
      }

      if (needRead) { readConfig(channel, chSettings); }
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
    value = analogRead(A6);
    press = presstime[0];
  }
  else
  {
    value = analogRead(A7);
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
  afk_time = 0;
  lineVar = 0;
  line = 0;
  pageVar = 0;
  oled.setPage(newState);

  timeCounter = 0;
}

void GUI::readConfig(byte channel, byte value[])
{
  store->readBytes(SAVENAME, channel, 0, 9, value);
}
void GUI::writeConfig(byte channel, byte value[])
{
  store->writeBytes(SAVENAME, channel, 0, 9, value);
  *needReload = true;
}

void GUI::getByteFormat(float data, byte index, byte array[])
{
  array[index] = ((byte)abs(data) % 128) + (data < 0 ? 128 : 0);
  array[index+1] = (abs(data) - (int)(abs(data))) * 256;
}
float GUI::reverseByteFormat(byte index, byte array[])
{
  float value = 0;
  value += array[index] % 128;
  value += (float)(array[index+1]) / 256;
  value *= (array[index] >= 128 ? -1 : 1);
  
  return value;
}
void GUI::getUnsignedByteFormat(float data, byte index, byte array[])
{
  array[index] = ((byte)abs(data) % 256);
  array[index+1] = (abs(data) - (int)(abs(data))) * 256;
}
float GUI::reverseUnsignedByteFormat(byte index, byte array[])
{
  float value = 0;
  value += array[index];
  value += (float)(array[index+1]) / 256;
  
  return value;
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

void GUI::message(byte fsize, String message)
{
  oled.clear();
  oled.drawText(oled.getCurPage(), (128-(fsize*6*message.length()))/2, (32-(fsize*6))/2, message, fsize);
  oled.show();
  delay(1000);
}