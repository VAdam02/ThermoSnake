/*
  GUI.h - Library to control GUI
*/

#ifndef GUI_h
#define GUI_h

#include <Arduino.h>
#include "../../src/Graphics/Graphics.h"

class GUI
{
  public:
    GUI();
    void GUI::begin(float *_sensors[]);
    void GUI::refresh(unsigned int deltatime);
    void GUI::endrefresh();
  private:
    Graphics oled;
    byte state = 0;
    byte nextState = 0;
    unsigned int timeCounter = 0;
    float **sensors;
    unsigned int presstime[2] = {0, 0};
    float pageSwitcher = 0;

    void GUI::getJoyStick(unsigned int deltatime, float *data, bool horizontal);
    void GUI::setState(byte newState);
    long GUI::exponentiation(long base, long exponent);
    String GUI::numToString(double num, int decimals);
};

#endif