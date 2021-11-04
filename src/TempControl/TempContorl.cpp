/*
 * EEPROM allocations:
 * B 0 - for channel 0's variables
 * B 1 - for channel 1's variables
 */

#include <Arduino.h>

TempAndHum* sensor0;
Backstore* store;

TempControl::TempControl() { }

void TempControl::begin(TempAndHum* tempAndHum, Backstore* backstore)
{
    sensor0 = tempAndHum;
    store = backstore;
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

byte[] TempControl::getByteFormat(double data)
{
  byte value[];

  unsigned int fullPart = (unsigned int)abs(data);
  unsigned double otherPart = abs(data) - fullPart;

  fullPart = fullPart % 128;
  byte current = 128;
  byte cache = 0;
  //negativa = 1 positive = 0
  if (data < 0) { cache++; }
  //full part
  while (current >= 1)
  {
    cache *= 2;
    current /= 2;

    if (fullPart >= current)
    {
      cache++;
      fullPart -= current;
    }
  }
  value[0] = cache;
  cache = 0;

  //other part
  int i = 0;
  double current2 = 1;
  while (i < 8)
  {
    cache *= 2;
    current2 /= 2;

    if (otherpart >= current2)
    {
      cache++;
      otherPart -= current2;
    }

    i++;
  }
  value[1] = cache;
}

void TempControl::readConfig()
{
    //read channel0's config
    byte mode[1];
    if (*store.readBytes('B', 0, 0, 0, data) != 0)
    {
        //error - not found so inicialise to null
        *store.allocateSpace('B', 0, 1);
        *store.writeBytes('B', 0, 0, 0, {0});
        mode[0] = 0;
    }
    
    if (mode == 0)
    {
      //do nothing
    }
    else if (mode == 1)
    {
      //read mode1's parameters
    }
    else if (mode == 2)
    {
      //read mode2's parameters
    }
}