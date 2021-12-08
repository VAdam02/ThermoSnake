/*
  Backstore.h - Library to manage EEPROM
*/

#ifndef Backstore_h
#define Backstore_h

#include <Arduino.h>
#include <Wire.h>

class Backstore
{
  public:
    Backstore();
    void Backstore::begin();

    byte Backstore::freeUpSpace(char c, byte num);
    byte Backstore::allocateSpace(char c, byte num, byte size);

    byte Backstore::readBytes(char c, byte num, byte first, byte last, byte data[]);
    byte Backstore::writeBytes(char c, byte num, byte first, byte last, byte data[]);

    void Backstore::regroupFreeSpace();
    void Backstore::defragmentStorage();

    void Backstore::inicialise(byte headSize);
    void Backstore::mem();
  private:
    char chars[25] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y'};
    
    void Backstore::write(unsigned int address, byte data);
    byte Backstore::read(unsigned int address);

    byte Backstore::getDataSize();
    void Backstore::addDataSize(int dif);
    byte Backstore::getFreeSize();
    void Backstore::addFreeSize(int dif);
    byte Backstore::getHeadSize();

    byte Backstore::nameToNum(char c, byte num);
    byte Backstore::nameToAllocationNoteIndex(char c, byte num);
    bool Backstore::setLazy(unsigned int address, byte value);
    void Backstore::setLazy4(unsigned int address, byte val0, byte val1, byte val2, byte val3);

    byte Backstore::moveNote(byte first, byte last, int moveby);
    byte Backstore::removeFreeNote(byte index);
    byte Backstore::insertFreeNote(unsigned int address, byte size);
    byte Backstore::removeAllocationNote(byte index);
    byte Backstore::insertAllocationNote(unsigned int address, byte size, byte name);
};

#endif