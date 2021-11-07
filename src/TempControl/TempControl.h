/*
  TempControl.h - Library to control heating devices
*/

#ifndef TempControl_h
#define TempControl_h

#include "Arduino.h"
#include "../../src/TempAndHum/TempAndHum.h"
#include "../../src/Backstore/Backstore.h"

class TempControl
{
  public:
    TempControl();
    void TempControl::begin(TempAndHum* tempAndHum, Backstore* backstore);
    void TempControl::getByteFormat(double data, byte returnValue[]);
    void TempControl::readConfig();
  private:
    TempAndHum* sensor0;
    Backstore* store;
};

#endif