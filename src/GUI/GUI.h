/*
  GUI.h - Library to control GUI
*/

#ifndef GUI_h
#define GUI_h

#include <Arduino.h>
#include "../../src/Graphics/Graphics.h"
#include "../../src/Backstore/Backstore.h"
#include "../../src/TempControl/TempControl.h"

class GUI
{
  public:
    GUI();
    void GUI::begin(bool *_needReload, TempControl *tempControl, Backstore *_store, float *_TempSensors[], float *_HumSensors[]);
    void GUI::refresh(unsigned int deltatime);
    void GUI::endrefresh();
  private:
    bool *needReload;
    Graphics oled;
    Backstore *store;
    TempControl *tempControl;
    unsigned int timeCounter = 0;
    float **TempSensors;
    float **HumSensors;
    unsigned int presstime[2] = {0, 0};
    float pageVar = 0;
    float lineVar = 0;
    byte line = 0;
    byte lineCount = 1;
    float afk_time = 0;

    byte chSettings[10];
    byte channel = 0;

    void GUI::getJoyStick(unsigned int deltatime, float *data, bool horizontal);
    void GUI::setState(byte newState);

    void GUI::readConfig(byte channel, byte value[]);
    void GUI::writeConfig(byte channel, byte value[]);
    void GUI::restart();

    void GUI::getByteFormat(float data, byte index, byte array[]);
    float GUI::reverseByteFormat(byte index, byte array[]);
    void GUI::getUnsignedByteFormat(float data, byte index, byte array[]);
    float GUI::reverseUnsignedByteFormat(byte index, byte array[]);

    void GUI::message(byte fsize, String message);
    long GUI::exponentiation(long base, long exponent);
    String GUI::numToString(double num, int decimals);
};

#endif