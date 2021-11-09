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
    void TempControl::begin(float *_sensors[], Backstore* _store);
    void TempControl::refresh();

    void TempControl::addHeatingTask(byte ID, unsigned int on_time, byte maxDelay_time);
    void TempControl::stopHeatingTask(byte ID);

    bool TempControl::level0(byte channel, float curLevel);
    bool TempControl::level1(byte channel, float curLevel, unsigned int deltatime);
    void TempControl::getByteFormat(float data, byte returnValue[]);
    float TempControl::reverseByteFormat(byte data[]);
    byte chanelParams[2][21];
    float **sensors;
  private:
    
    Backstore *store;
    unsigned int lastTime = 0;
    
    void TempControl::readConfig();
};

#endif