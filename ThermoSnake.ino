#include "src/DelayManager/DelayManager.h"
#include <EEPROM.h>

DelayManager delayer;

void setup()
{
  delayer.begin();
  Serial.begin(9600);
  /*
  Serial.print("Clear");
  erase();
  */
  Serial.print("\n");
  Serial.print("Start");
  //inicialise(64);
  /*
  mem();
  Serial.print("\n");
  allocate('B', 5, 60);
  Serial.print("\n");
  mem();
  Serial.print("\n");
  allocate('B', 4, 240);
  Serial.print("\n");
  mem();
  Serial.print("\n");
  allocate('B', 3, 25);
  Serial.print("\n");
  mem();
  Serial.print("\n");
  allocate('B', 2, 230);
  Serial.print("\n");
  */
  mem();
  Serial.print("\n");
  byte data[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
  Serial.print(write('B', 3, 10, data, 20));
  Serial.print("\n");
  mem();
}

void loop()
{
  /*
  mem();
  delayer.sleepReamingOf(10000);
  */
}

const char chars[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y'};

bool write(char c, byte num, byte offset, byte data[], byte length)
{
  byte a = 0;
  while (a < 25 && !(c == chars[a])) { a++; }
  if (a >= 25) { return false; }
  
  if (num > 9) { return false; }
  a = (a*10) + num;

  int i = 1;
  while (i < getDataEnd()+1 && !(a == EEPROM.read(i*4+3))) { i++; }
  if (!(i < getDataEnd() + 1)) { return false; }

  int address = EEPROM.read(i*4) * 256 + EEPROM.read(i*4+1);
  byte size = EEPROM.read(i*4+2);

  i = 0;
  while (i + offset < size && i < length)
  {
    setLazy(address + i + offset, data[i]);
    i++;
  }
  return true;
}

bool allocate(char c, byte num, byte size)
{
  byte a = 0;
  while (a < 25 && !(c == chars[a])) { a++; }
  if (a >= 25) { return false; }
  
  if (num > 9) { return false; }
  a = (a*10) + num;

  //look for free space
  int i = 0;
  while (i < (getFreeStart()+1) && !(EEPROM.read((getHeadSize()-i)*4+2) == 0 || (EEPROM.read((getHeadSize()-i)*4+2) >= size))) { i++; }
  if (i >= (getFreeStart()+1)) { return false; }
  
  int address = EEPROM.read((getHeadSize()-i)*4) * 256 + EEPROM.read((getHeadSize()-i)*4+1);
  
  //modify the free space
  setLazy(((getHeadSize()-i)*4) * 256 + ((getHeadSize()-i)*4), (address + size) >> 8);
  setLazy(((getHeadSize()-i)*4) * 256 + ((getHeadSize()-i)*4+1), (address + size) & 0xFF);
  if (EEPROM.read((getHeadSize()-i)*4+2) > 0)
  {
    //TODO if the full space filled than remove
    setLazy((getHeadSize()-i)*4+2, EEPROM.read((getHeadSize()-i)*4+2) - size);
  }
  else
  {
    if (EEPROM.length() - (address + size) < 256) { setLazy((getHeadSize()-i)*4+2, EEPROM.length() - (address + size)); }
    else { setLazy((getHeadSize()-i)*4+2, 0); }
  }

  //look where it should be in the order
  i = 1;
  while (i < getDataEnd()+1 && !(a <= EEPROM.read(i*4+3))) { i++; }
  if (i < getDataEnd()+1)
  {
    for (int j = (getDataEnd()+1)*4-1; j >= i*4; j--)
    {
      setLazy(j+4, EEPROM.read(j));
    }
  }

  //add allocation to table
  dataEnd(1);
  setLazy(i * 4, address >> 8);
  setLazy(i * 4+1, address & 0xFF);
  setLazy(i * 4+2, size);
  setLazy(i * 4+3, a);
  
  return true;
}

void inicialise(byte headSize)
{
  for (int i = 0 ; i < EEPROM.length() ; i++)
  {
    setLazy(i, 0);
  }

  setDataEndLazy(0);
  setFreeStartLazy(0);
  setLazy(2, headSize-1);

  //rest free space
  setLazy((headSize-1)*4, (headSize*4) >> 8);
  setLazy((headSize-1)*4+1, (headSize*4) & 0xFF);
  if (EEPROM.length() - (headSize*4) < 256) { setLazy((headSize-1)*4+2, EEPROM.length() - (headSize*4)); }
  else { setLazy((headSize-1)*4+2, 0); }
  setLazy((headSize-1)*4+3, 254);
  
}


byte getHeadSize()
{
  return EEPROM.read(2);
}
bool setLazy(unsigned int addr, byte val)
{
  if (EEPROM.read(addr) != val)
  {
    EEPROM.write(addr, val);
    return true;
  }
  return false;
}

bool setFreeStartLazy(byte val)
{
  if (getFreeStart() != val)
  {
    EEPROM.write(1, val);
    return true;
  }
  return false;
}
void freeStart(int dif)
{
  EEPROM.write(1, getDataEnd() + dif);
}
byte getFreeStart()
{
  return EEPROM.read(1);
}

bool setDataEndLazy(byte val)
{
  if (getDataEnd() != val)
  {
    EEPROM.write(0, val);
    return true;
  }
  return false;
}
void dataEnd(int dif)
{
  EEPROM.write(0, getDataEnd() + dif);
}
byte getDataEnd()
{
  return EEPROM.read(0);
}




void mem()
{
  Serial.print("\n");
  for (int i = 0; i < EEPROM.length()/16; i++)
  {
    Serial.print("\n");
    Serial.print(i);
    Serial.print("\t");
    for (int j = 0; j < 16; j++)
    {
      int a = i*16+j;
      Serial.print(EEPROM.read(a));
      Serial.print(" ");
    }
  }
  Serial.print("\n");
}
