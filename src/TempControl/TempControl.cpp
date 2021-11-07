/*
 * EEPROM allocations:
 * B 0 - for channel 0's variables
 * B 1 - for channel 1's variables
 */

#include "TempControl.h"
#include <Arduino.h>

TempControl::TempControl() { }

void TempControl::begin(TempAndHum* _sensor0, Backstore* _store)
{
    sensor0 = _sensor0;
    store = _store;
}

/*
  0-1 bit - mode
    00 - null
    01 - level 0
      offLevel
      onLevel
    10 - level 1
    11 - unused
  
 */

void TempControl::getByteFormat(double data, byte returnValue[])
{
  unsigned int fullPart = (unsigned int)abs(data);
  double otherPart = abs(data) - fullPart;

  fullPart = fullPart % 128;
  byte current = 128;
  byte cache = 0;
  //negativa = 1 positive = 0
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

void TempControl::readConfig()
{
    //read channel0's config
    byte mode[1];
    if (store->readBytes('B', 0, 0, 0, mode) != 0)
    {
      Serial.print("Inicialise\n");
      //error - not found so inicialise to null mode
      store->allocateSpace('B', 0, 1);
      store->writeBytes('B', 0, 0, 0, {0});
      mode[0] = 0;
    }
    
    Serial.print(mode[0]);
    if (mode[0] == 0)
    {
      Serial.print("null\n");
      //do nothing
    }
    else if (mode[0] == 1)
    {
      Serial.print("level0\n");
      //read mode1's parameters
    }
    else if (mode[0] == 2)
    {
      Serial.print("level1\n");
      //read mode2's parameters
    }
}