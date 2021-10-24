#include "src/DelayManager/DelayManager.h"
#include <EEPROM.h>

DelayManager delayer;

void setup()
{
  delayer.begin();
  Serial.begin(9600);
  
  Serial.print("\n");
  Serial.print("Start");
  inicialise(64);
  mem();
  
  
  Serial.print("\n");
  Serial.print(allocateSpace('B', 2, 160));
  Serial.print("\n");
  mem();
  
  Serial.print("\n");
  Serial.print(allocateSpace('B', 3, 150));
  Serial.print("\n");
  mem();

  Serial.print("\n");
  Serial.print(allocateSpace('B', 4, 140));
  Serial.print("\n");
  mem();
  
  Serial.print("\n");
  Serial.print(allocateSpace('B', 5, 130));
  Serial.print("\n");
  mem();

  Serial.print("\n");
  Serial.print(allocateSpace('B', 6, 160));
  Serial.print("\n");
  mem();

  Serial.print("\n");
  Serial.print(freeUpSpace('B', 5));
  Serial.print("\n");
  mem();
  
  Serial.print("\n");
  Serial.print(freeUpSpace('B', 2));
  Serial.print("\n");
  mem();

  Serial.print("\n");
  Serial.print(freeUpSpace('B', 3));
  Serial.print("\n");
  mem();

  Serial.print("\n");
  Serial.print(freeUpSpace('B', 6));
  Serial.print("\n");
  mem();


  optimizeStorage();
  mem();

  /*
  Serial.print("\n");
  allocateSpace('B', 2, 10);
  Serial.print("\n");
  mem();
  */
  

  /*
  Serial.print("\n");
  byte data[] = {analogRead(A0), analogRead(A1), analogRead(A2), analogRead(A3), analogRead(A4), analogRead(A5), analogRead(A6), 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
  Serial.print(writeBytes('B', 2, 0, 20, data));
  for (int i = 0; i < 16; i++)
  {
    data[i] = 43;
  }
  Serial.print("\n");
  mem();
  
  
  Serial.print("\n");
  byte asd = 20;
  byte data2[asd];
  Serial.print(readBytes('B', 2, 5, asd-1, data2));
  Serial.print("\n");
  for (int i = 0; i < asd; i++)
  {
    Serial.print(data2[i]);
    Serial.print(" ");
  }
  Serial.print("\n");
  mem();
  */
}

void loop()
{
  /*
  mem();
  delayer.sleepReamingOf(10000);
  */
}

const char chars[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y'};

/*
 * RETURN VALUE
 * 0-24 - success
 * 25 - illegal char
 */
byte nameToNum(char c, byte num)
{
  byte a = 0;
  while (a < 25 && !(c == chars[a])) { a++; }
  if (a >= 25) { a = 25; }
  
  a = (a*10) + (num%10);
  return a;
}

byte getDataSize() { return EEPROM.read(0); }
void addDataSize(int dif)
{
  setLazy(0, getDataSize() + dif);
}

byte getFreeSize() { return EEPROM.read(1); }
void addFreeSize(int dif)
{
  setLazy(1, getFreeSize() + dif);
}

byte getHeadSize() { return EEPROM.read(2); }

bool setLazy(unsigned int address, byte value)
{
  if (EEPROM.read(address) != value)
  {
    EEPROM.write(address, value);
    return true;
  }
  return false;
}

/*
 * RETURN VALUE
 * 0 - success
 * 1 - success but nothing changed
 */
byte moveNote(byte first, byte last, int moveby)
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
  for (byte i = first; i != last+direction; i += direction)
  {
    setLazy((unsigned int)(i+moveby)*4, EEPROM.read(i*4));
    setLazy((unsigned int)(i+moveby)*4+1, EEPROM.read(i*4+1));
    setLazy((unsigned int)(i+moveby)*4+2, EEPROM.read(i*4+2));
    setLazy((unsigned int)(i+moveby)*4+3, EEPROM.read(i*4+3));
  }
  return 0;
}

/*
 * RETURN VALUE
 * 0 - success, just removed
 * 1 - success, moved
 */
byte removeFreeNote(byte index)
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
byte insertFreeNote(unsigned int address, byte size)
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
    taddress = EEPROM.read((headSize-i)*4) * 256 + EEPROM.read((headSize-i)*4+1);
    tsize = EEPROM.read((headSize-i)*4+2);
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
    byte ttype = EEPROM.read((headSize-i)*4+3);
    if (ttype == 254 && (tsize + size) > 255)
    {
      setLazy((headSize-i)*4+2, 0);
    }
    else if ((tsize + size) > 255)
    {
      setLazy((headSize-i)*4+2, 255);

      /* register a new note
       * address + 255
       * size + tsize - 255
       * ttype
       */
      Serial.print("\n");
      Serial.print(i);
      Serial.print(" ");
      Serial.print(taddress);
      Serial.print(" ");
      Serial.print(taddress+255);
      Serial.print("\n");
      
      moveNote(headSize-(i+1), headSize-(freeSize+1), -1);
      
      setLazy((headSize-(i+1))*4, (taddress + 255) >> 8);
      setLazy((headSize-(i+1))*4+1, (taddress + 255) & 0xFF);
      setLazy((headSize-(i+1))*4+2, size + tsize - 255);
      setLazy((headSize-(i+1))*4+3, ttype);

      addFreeSize(1);
      //TODO error because rollover
      //TODO may it will rollover at the size increment so a new note should be added
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
      taddress = EEPROM.read((getHeadSize()-i)*4)*256 + EEPROM.read((getHeadSize()-i)*4+1);
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
 * 0 - not found
 * 1-255 - found
 */
byte nameToAllocationNoteIndex(char c, byte num)
{
  byte dataSize = getDataSize();
  byte a = nameToNum(c, num);
  
  byte name = 0;
  byte i = 0;
  do
  {
    i++;
    name = EEPROM.read(i*4+3);
  }
  while (i <= dataSize && !(a == name));
  if (!(i <= dataSize)) { return 0; }
  return i;
}

/*
 * RETURN VALUE
 * 0 - success, just removed
 * 1 - success, moved
 */
byte removeAllocationNote(byte index)
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
byte insertAllocationNote(unsigned int address, byte size, byte name)
{
  byte dataSize = getDataSize();
  
  //look where it should be in the order
  byte returnValue = 0;
  byte tname;
  int i = 0;
  do
  {
    i++;
    tname = EEPROM.read(i*4+3);
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
byte freeUpSpace(char c, byte num)
{
  byte dataSize = getDataSize();

  //look for the allocation note
  byte index = nameToAllocationNoteIndex(c, num);
  if (index == 0) { return 30; }

  //save the parameters
  unsigned int address = EEPROM.read(index*4) * 256 + EEPROM.read(index*4+1);
  byte size = EEPROM.read(index*4+2);

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
byte allocateSpace(char c, byte num, byte size)
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
    tsize = EEPROM.read((headSize-i)*4+2);
    ttype = EEPROM.read((headSize-i)*4+3);
  }
  while (i <= freeSize && !((tsize == 0 && ttype == 254) || (tsize >= size)));
  if (i > freeSize) { return 3; }

  unsigned int taddress = (headSize-i)*4;
  unsigned int address = EEPROM.read(taddress) * 256 + EEPROM.read(taddress+1);

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
      if (EEPROM.length() - (address + size) < 256) { setLazy(taddress+2, EEPROM.length() - (address + size)); }
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
byte readBytes(char c, byte num, byte first, byte last, byte data[])
{
  byte dataSize = getDataSize();
  byte index = nameToAllocationNoteIndex(c, num);
  
  unsigned int address = EEPROM.read(index*4) * 256 + EEPROM.read(index*4+1);
  byte size = EEPROM.read(index*4+2);
  
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
    data[i-first] = EEPROM.read(address+i);
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
byte writeBytes(char c, byte num, byte first, byte last, byte data[])
{
  byte dataSize = getDataSize();
  byte index = nameToAllocationNoteIndex(c, num);

  unsigned int address = EEPROM.read(index*4) * 256 + EEPROM.read(index*4+1);
  byte size = EEPROM.read(index*4+2);

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

void optimizeStorage()
{
  byte freeSize = getFreeSize();
  byte headSize = getHeadSize();
  
  //TODO group free space
  unsigned address0 = EEPROM.read(headSize*4) * 256 + EEPROM.read(headSize*4+1);
  byte size0 = EEPROM.read(headSize*4+2);
  byte type0 = EEPROM.read(headSize*4+3);

  Serial.print(".0 ");
  
  int i = 0;
  while (i < freeSize)
  {
    Serial.print(".1 ");
    unsigned address1 = EEPROM.read((headSize - (i+1))*4) * 256 + EEPROM.read((headSize - (i+1))*4+1);
    byte size1 = EEPROM.read((headSize - (i+1))*4+2);
    byte type1 = EEPROM.read((headSize - (i+1))*4+3);

    //TODO 254's size
    if (address0 + size0 == address1)
    {
      Serial.print(".2 ");
      //attach 0 and 1 free space notes beacause they next to each other
      if (size0+size1 <= 255 || type1 == 254)
      {
        Serial.print(".3 ");
        //create one big part
        if (type1 == 254 && (size1 == 0 || size0+size1 > 255))
        {
          Serial.print(".4 ");
          //calculate the space specially
          if ((EEPROM.length() - address0) > 255)
          {
            Serial.print(".5 ");
            setLazy((headSize - i)*4+2, 0);
          }
          else
          {
            Serial.print(".6 ");
            setLazy((headSize - i)*4+2, (EEPROM.length() - address0));
          }
        }
        else
        {
          Serial.print(".7 ");
          setLazy((headSize - i)*4+2, size0+size1);
        }
        
        setLazy((headSize - i)*4+3, type1);
        removeFreeNote(i+1);

        
        freeSize -= 1;
        i--;
        address1 = address0; //keep the smaller address
        size1 += size0; //keep both of the size

        //TODO there should be a problem due to modify i and freeSize
      }
      else
      {
        Serial.print(".8 ");
        //create a full and a small part
        //full
        setLazy((headSize - i)*4+2, 255);
        //not full
        setLazy((headSize - (i+1))*4+2, size0+size1);
      }
    }
    Serial.print(".9 ");

    address0 = address1;
    size0 = size1;
    type0 = type1;
    i++;
    Serial.print("\n");
    Serial.print("\n");
    Serial.print(i);
    Serial.print(" round");
    Serial.print("\n");
    mem();
  }
  //TODO rearrange allocations
}

void inicialise(byte headSize)
{
  /*
  for (int i = 0 ; i < headSize*4 ; i++)
  {
    setLazy(i, 0);
  }
  */
  
  for (int i = 0 ; i < EEPROM.length(); i++)
  {
    setLazy(i, 0);
  }
  
  
  setLazy(0, 0);
  setLazy(1, 0);
  setLazy(2, headSize-1);

  //rest free space
  setLazy((headSize-1)*4, (headSize*4) >> 8);
  setLazy((headSize-1)*4+1, (headSize*4) & 0xFF);
  if (EEPROM.length() - (headSize*4) < 256) { setLazy((headSize-1)*4+2, EEPROM.length() - (headSize*4)); }
  else { setLazy((headSize-1)*4+2, 0); }
  setLazy((headSize-1)*4+3, 254);
  
}




void mem()
{
  byte width = 32;
  for (int i = 0; i < EEPROM.length()/width; i++)
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

      byte cur = EEPROM.read(a);
      if (cur < 100) { Serial.print(" "); }
      if (cur < 10) { Serial.print(" "); }
      Serial.print(" ");
      Serial.print(cur);
    }
  }
  Serial.print("\n");
}
