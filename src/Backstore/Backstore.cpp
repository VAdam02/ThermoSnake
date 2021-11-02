#include "Backstore.h"
#include <Arduino.h>
#include <Wire.h>

#define EEPROM_SIZE 4096
#define EEPROM_ADDRESS 0x50

Backstore::Backstore() { }

void Backstore::begin()
{
  Wire.begin();
}

void Backstore::write(unsigned int address, byte data)
{
  Wire.beginTransmission(EEPROM_ADDRESS);
  Wire.write((int)(address >> 8));
  Wire.write((int)(address & 0xFF));
  Wire.write(data);
  Wire.endTransmission();
  delay(10);

  //TODO check back the data is correct
}

byte Backstore::read(unsigned int address)
{
  byte data = 0xFF;
  Wire.beginTransmission(EEPROM_ADDRESS);
  Wire.write((int)(address >> 8));
  Wire.write((int)(address & 0xFF));
  Wire.endTransmission();
  Wire.requestFrom(EEPROM_ADDRESS,1);
  if (Wire.available())
  {
    data = Wire.read();
  }
  return data;
}

byte Backstore::getDataSize() { return read(0); }
void Backstore::addDataSize(int dif)
{
  setLazy(0, getDataSize() + dif);
}

byte Backstore::getFreeSize() { return read(1); }
void Backstore::addFreeSize(int dif)
{
  setLazy(1, getFreeSize() + dif);
}

byte Backstore::getHeadSize() { return read(2); }

/*
 * RETURN VALUE
 * 0-24 - success
 * 25 - illegal char
 */
byte Backstore::nameToNum(char c, byte num)
{
  byte a = 0;
  while (a < 25 && !(c == chars[a])) { a++; }
  if (a >= 25) { a = 25; }
  
  a = (a*10) + (num%10);
  return a;
}

/*
 * RETURN VALUE
 * 0 - not found
 * 1-255 - found
 */
byte Backstore::nameToAllocationNoteIndex(char c, byte num)
{
  byte dataSize = getDataSize();
  byte a = nameToNum(c, num);
  
  byte name = 0;
  byte i = 0;
  do
  {
    i++;
    name = read(i*4+3);
  }
  while (i <= dataSize && !(a == name));
  if (!(i <= dataSize)) { return 0; }
  return i;
}

bool Backstore::setLazy(unsigned int address, byte value)
{
  if (read(address) != value)
  {
    write(address, value);
    return true;
  }
  return false;
}

/*
 * RETURN VALUE
 * 0 - success
 * 1 - success but nothing changed
 */
byte Backstore::moveNote(byte first, byte last, int moveby)
{
  if (moveby == 0) { return 1; }
  if (moveby > 0 && first < last)
  {
    byte cache = first;
    first = last;
    last = cache;
  }
  if (moveby < 0 && first > last)
  {
    byte cache = first;
    first = last;
    last = cache;
  }

  //move first by count than the middle than the last
  int direction = (last-first) / abs(last-first);
  for (byte i = first; i != (byte)(last+direction); i += direction)
  {
    setLazy((unsigned int)(i+moveby)*4, read(i*4));
    setLazy((unsigned int)(i+moveby)*4+1, read(i*4+1));
    setLazy((unsigned int)(i+moveby)*4+2, read(i*4+2));
    setLazy((unsigned int)(i+moveby)*4+3, read(i*4+3));
  }
  return 0;
}

/*
 * RETURN VALUE
 * 0 - success, just removed
 * 1 - success, moved
 */
byte Backstore::removeFreeNote(byte index)
{
  byte returnValue = 0;
  byte freeSize = getFreeSize();
  byte headSize = getHeadSize();
  
  if (index < freeSize)
  {
    returnValue = 1 - moveNote(headSize -freeSize, headSize - (index +1),1);
  }

  addFreeSize(-1);
  return returnValue;
}

/*
 * RETURN VALUE
 * 0 - success, extend
 * 1 - success, insert - may defragment is needed if it returned too much time
 */
byte Backstore::insertFreeNote(unsigned int address, byte size)
{
  byte freeSize = getFreeSize();
  byte headSize = getHeadSize();

  //find the ideal place in the list that ordered by the memory address
  int i = -1;
  unsigned int taddress = 0;
  unsigned int tsize = 0;
  do
  {
    i++;
    taddress = read((headSize-i)*4) * 256 + read((headSize-i)*4+1);
    tsize = read((headSize-i)*4+2);
  }
  while (i <= freeSize && !(taddress + tsize == address || taddress == address + size));

  //extend
  if (i <= freeSize)
  {
    //set the new space as the begining of the free space if the next free space is exactly next to it
    if (address + size == taddress)
    {
      setLazy((headSize-i)*4, address >> 8);
      setLazy((headSize-i)*4+1, address & 0xFF);
    }
    
    //set the new size
    //if its the reaming part of storage than it should be 0 or 1-255 if the left storage is going to be small
    byte ttype = read((headSize-i)*4+3);
    if (ttype == 254 && (tsize + size) > 255)
    {
      setLazy((headSize-i)*4+2, 0);
    }
    else if ((tsize + size) > 255)
    {
      setLazy((headSize-i)*4+2, 255);
      
      moveNote(headSize-(i+1), headSize-(freeSize+1), -1);
      
      setLazy((headSize-(i+1))*4, (taddress + 255) >> 8);
      setLazy((headSize-(i+1))*4+1, (taddress + 255) & 0xFF);
      setLazy((headSize-(i+1))*4+2, size + tsize - 255);
      setLazy((headSize-(i+1))*4+3, ttype);

      addFreeSize(1);
    }
    else
    {
      setLazy((headSize-i)*4+2, tsize + size);
    }
    return 0;
  }
  //insert
  else
  {
    //look for the first note where the address is greater than my address
    i = -1;
    do
    {
      i++;
      taddress = read((getHeadSize()-i)*4)*256 + read((getHeadSize()-i)*4+1);
    }
    while (i < freeSize && !(taddress > address));

    moveNote(headSize-i, headSize-freeSize, -1);

    setLazy((headSize-i)*4, address >> 8);
    setLazy((headSize-i)*4+1, address & 0xFF);
    setLazy((headSize-i)*4+2, size);
    setLazy((headSize-i)*4+3, 255);

    addFreeSize(1);
    return 1;
  }
}

/*
 * RETURN VALUE
 * 0 - success, just removed
 * 1 - success, moved
 */
byte Backstore::removeAllocationNote(byte index)
{
  byte returnValue = 0;
  byte dataSize = getDataSize();
  
  if (index < dataSize)
  {
    returnValue = 1 - moveNote(index+1, dataSize, -1);
  }
  
  addDataSize(-1);
  return returnValue;
}

/*
 * RETURN VALUE
 * 0 - success, inserted at the end
 * 1 - success, inserted between two note
 */
byte Backstore::insertAllocationNote(unsigned int address, byte size, byte name)
{
  byte dataSize = getDataSize();
  
  //look where it should be in the order
  byte returnValue = 0;
  byte tname;
  int i = 0;
  do
  {
    i++;
    tname = read(i*4+3);
  }
  while (i <= dataSize && !(name <= tname));
  if (i <= dataSize)
  {
    returnValue = 1 - moveNote(i, dataSize, 1);
  }

  //add allocation to table
  setLazy(i * 4, address >> 8);
  setLazy(i * 4+1, address & 0xFF);
  setLazy(i * 4+2, size);
  setLazy(i * 4+3, name);
  addDataSize(1);
  return returnValue;
}







/*
 * RETURN VALUE
 * X0 - one of the free space note extended
 * X1 - new one of free space note inserted
 * 0X - removed from the end of the data notes
 * 1X - removed and moved the data notes to fill hole
 * 30 - not found
 */
byte Backstore::freeUpSpace(char c, byte num)
{
  byte dataSize = getDataSize();

  //look for the allocation note
  byte index = nameToAllocationNoteIndex(c, num);
  if (index == 0) { return 30; }

  //save the parameters
  unsigned int address = read(index*4) * 256 + read(index*4+1);
  byte size = read(index*4+2);

  byte returnValue = 0;
  //remove allocation from header
  returnValue = removeAllocationNote(index) * 10;

  //register free space
  returnValue += insertFreeNote(address, size);

  return returnValue;
}

/*
 * RETURN VALUE
 * 0 - allocated at the end of the data notes
 * 1 - allocated between two data notes
 * 2 - illegal name
 * 3 - no free space that big enough
 */
byte Backstore::allocateSpace(char c, byte num, byte size)
{
  byte headSize = getHeadSize();
  byte freeSize = getFreeSize();
  
  byte a = nameToNum(c, num);
  if (a == 25) { return 2; }

  //look for free space that big enough
  byte tsize = 0;
  byte ttype = 0;
  byte i = 255; //rollover at first cycle so it means -1
  do
  {
    i++;
    tsize = read((headSize-i)*4+2);
    ttype = read((headSize-i)*4+3);
  }
  while (i <= freeSize && !((tsize == 0 && ttype == 254) || (tsize >= size)));
  if (i > freeSize) { return 3; }

  unsigned int taddress = (headSize-i)*4;
  unsigned int address = read(taddress) * 256 + read(taddress+1);

  //remove this note because it's size is 0
  if (tsize == size && ttype != 254) { removeFreeNote(i); }
  else
  {
    //incrase the address
    setLazy(taddress, (address + size) >> 8);
    setLazy(taddress+1, (address + size) & 0xFF);

    //modify the remaining free space at the note
    if (ttype == 254)
    {
      //if it's the remaining part of the EEPROM than it should have a special size
      if (EEPROM_SIZE - (address + size) < 256) { setLazy(taddress+2, EEPROM_SIZE - (address + size)); }
      else { setLazy(taddress+2, 0); }
    }
    else
    {
      //decrase space allocation size and modify start address
      setLazy(taddress+2, tsize-size);
    }
  }
  
  //TODO manage if it want to reallocate with same name
  byte returnValue = 0;
  returnValue = insertAllocationNote(address, size, a);

  return returnValue;
}

/*
 * RETURN VALUE
 * 0 - success
 * 1 - succes but it point outside the allocation
 * 2 - not found
 */
byte Backstore::readBytes(char c, byte num, byte first, byte last, byte data[])
{
  byte dataSize = getDataSize();
  byte index = nameToAllocationNoteIndex(c, num);
  
  unsigned int address = read(index*4) * 256 + read(index*4+1);
  byte size = read(index*4+2);
  
  if (index == 0)
  {
    for (int i = 0; i < (last+1)-first; i++)
    {
      data[i] = 0;
    }
    return 2;
  }

  byte returnValue = 0;
  byte i = first;
  while (i <= last && i < size)
  {
    data[i-first] = read(address+i);
    i++;
  }
  while (i <= last)
  {
    data[i-first] = 0;
    i++;
    returnValue = 1;
  }

  return 0;
}

/*
 * RETURN VALUE
 * 0 - success
 * 1 - succes but it point outside the allocation
 * 2 - not found
 */
byte Backstore::writeBytes(char c, byte num, byte first, byte last, byte data[])
{
  byte dataSize = getDataSize();
  byte index = nameToAllocationNoteIndex(c, num);

  unsigned int address = read(index*4) * 256 + read(index*4+1);
  byte size = read(index*4+2);

  if (index == 0)
  {
    for (int i = 0; i < (last+1)-first; i++)
    {
      data[i] = 0;
    }
    return 2;
  }

  byte returnValue = 0;
  byte i = first;
  while (i <= last && i < size)
  {
    setLazy(address+i, data[i-first]);
    i++;
  }
  while (i <= last)
  {
    data[i-first] = 0;
    i++;
    returnValue = 1;
  }

  return 0;
}

void Backstore::regroupFreeSpace()
{
  byte freeSize = getFreeSize();
  byte headSize = getHeadSize();
  
  //group free space
  unsigned address0 = read(headSize*4) * 256 + read(headSize*4+1);
  byte size0 = read(headSize*4+2);
  byte type0 = read(headSize*4+3);
  
  int i = 0;
  while (i < freeSize)
  {
    unsigned address1 = read((headSize - (i+1))*4) * 256 + read((headSize - (i+1))*4+1);
    byte size1 = read((headSize - (i+1))*4+2);
    byte type1 = read((headSize - (i+1))*4+3);

    //type 254's size
    if (address0 + size0 == address1)
    {
      //attach 0 and 1 free space notes beacause they next to each other
      if (size0+size1 <= 255 || type1 == 254)
      {
        //create one big part
        if (type1 == 254 && (size1 == 0 || size0+size1 > 255))
        {
          //calculate the space specially
          if ((EEPROM_SIZE - address0) > 255)
          {
            setLazy((headSize - i)*4+2, 0);
          }
          else
          {
            setLazy((headSize - i)*4+2, (EEPROM_SIZE - address0));
          }
        }
        else
        {
          setLazy((headSize - i)*4+2, size0+size1);
        }
        
        setLazy((headSize - i)*4+3, type1);
        removeFreeNote(i+1);

        
        freeSize -= 1;
        i--;
        address1 = address0; //keep the smaller address
        size1 += size0; //keep both of the size
      }
      else
      {
        //create a full and a small part
        //full
        setLazy((headSize - i)*4+2, 255);
        //not full
        setLazy((headSize - (i+1))*4+2, size0+size1);
      }
    }

    address0 = address1;
    size0 = size1;
    type0 = type1;
    i++;
    //TODO debug
    mem();
  }
}

void Backstore::defragmentStorage()
{
  regroupFreeSpace();

  //TODO rearrange allocations
}

void Backstore::inicialise(byte headSize)
{
  int headSize_ = headSize;
  if (headSize == 0) { headSize_ = 256; }
  /*
  for (int i = 0 ; i < headSize_*4 ; i++)
  {
    setLazy(i, 0);
  }
  */
  
  for (int i = 0 ; i < EEPROM_SIZE; i++)
  {
    setLazy(i, 0);
  }
  
  
  setLazy(0, 0);
  setLazy(1, 0);
  setLazy(2, headSize_-1);

  //rest free space
  setLazy((headSize_-1)*4, (headSize_*4) >> 8);
  setLazy((headSize_-1)*4+1, (headSize_*4) & 0xFF);
  if (EEPROM_SIZE - (headSize_*4) < 256) { setLazy((headSize_-1)*4+2, EEPROM_SIZE - (headSize_*4)); }
  else { setLazy((headSize_-1)*4+2, 0); }
  setLazy((headSize_-1)*4+3, 254);
}

void Backstore::mem()
{
  byte width = 64;
  for (int i = 0; i < EEPROM_SIZE/width; i++)
  {
    if (i % 8 == 0)
    {
      Serial.print("\n");
    }
    Serial.print("\n");
    Serial.print(i);
    
    if (i < 10)
    {
      Serial.print(" ");
    }
    
    for (int j = 0; j < width; j++)
    {
      int a = i*width+j;
      if (a % 8 == 0 && j > 0)
      {
        Serial.print("|");
      }

      byte cur = read(a);
      if (cur < 100) { Serial.print(" "); }
      if (cur < 10) { Serial.print(" "); }
      Serial.print(" ");
      Serial.print(cur);
    }
  }
  Serial.print("\n");
}