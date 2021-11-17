/*
  GUI.h - Library to control GUI
*/

#ifndef GUI_h
#define GUI_h

#include <Arduino.h>
#include "../../src/Graphics/Graphics.h"
#include "../../src/Backstore/Backstore.h"

class GUI
{
  public:
    GUI();
    void GUI::begin(Backstore *_store, float *_TempSensors[], float *_HumSensors[]);
    void GUI::refresh(unsigned int deltatime);
    void GUI::endrefresh();
  private:
    Graphics oled;
    Backstore *store;
    unsigned int timeCounter = 0;
    float **TempSensors;
    float **HumSensors;
    unsigned int presstime[2] = {0, 0};
    float pageVar = 0;
    float lineVar = 0;
    byte line = 0;
    byte chSettings[21];

    void GUI::getJoyStick(unsigned int deltatime, float *data, bool horizontal);
    void GUI::setState(byte newState);
    long GUI::exponentiation(long base, long exponent);
    String GUI::numToString(double num, int decimals);
};

#endif