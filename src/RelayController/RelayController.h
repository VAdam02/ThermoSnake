/*
  RelayController.h - Library to manage timing of cycles
*/

#ifndef RelayController_h
#define RelayController_h

#include <Arduino.h>
#include "../../src/TempControl/TempControl.h"

class RelayController
{
  public:
    RelayController();
    void RelayController::begin(TempControl* _tempControl);
    void RelayController::refresh(unsigned int deltatime);
    unsigned int ch0 = 0;
    unsigned int ch1 = 0;
    unsigned int combo = 0;
  private:
    TempControl *tempControl;
    void RelayController::activate(byte channel);
};

#endif