/*
  Graphics.h - Library for interacting with OLED screen
*/

#ifndef Graphics_h
#define Graphics_h

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

class Graphics
{
  public:
    Graphics();
    void Graphics::begin();
    void Graphics::show();
    void Graphics::clear();
    void Graphics::drawDiagonal(byte absoluteX1, byte absoluteY1, byte relativeX2, float relativeY2, byte width, bool alignRight);
    void Graphics::drawQuarterCircle(byte x, byte y, byte width, byte direction);
    void Graphics::drawOuterQuarterCircle(byte x, byte y, byte width, byte direction);
    void Graphics::drawSquare(byte x, byte y, byte xwidth, byte yheight);
    void Graphics::setPage(byte page);
    byte Graphics::getCurPage();
    byte Graphics::getTargetPage();
    void Graphics::refresh();
    int Graphics::drawText(byte page, byte x, byte y, String text, byte fsize);
    int Graphics::drawChar(byte page, byte x, byte y, char c, byte fsize);
  private:
    unsigned int Graphics::getLocalTime(unsigned long time);
    byte curPage = 0;
    byte targetPage = 0;
    byte transitionX = 0;
    unsigned int lastTime = 0;
    int Graphics::getCurXByPageX(byte page, byte x);
    int Graphics::roundUp(double a);
    int Graphics::roundDown(double a);
};

#endif
