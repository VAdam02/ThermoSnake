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
    void TempControl::refresh();

    void TempControl::addHeatingTask(byte ID, unsigned int on_time, unsigned int maxDelay_time);
    void TempControl::stopHeatingTask(byte ID);

    bool TempControl::level0(byte channel, double curLevel);
    bool TempControl::level1(byte channel, double curLevel, unsigned int deltatime);
    void TempControl::getByteFormat(double data, byte returnValue[]);
    double TempControl::reverseByteFormat(byte data[]);
    byte chanelParams[2][18];
  private:
    TempAndHum* sensor0;
    Backstore* store;
    unsigned int lastTime = 0;
    
    void TempControl::readConfig();
};

#endif