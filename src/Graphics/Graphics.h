/*
  Graphics.h - Library for interacting with OLED screen
*/

#ifndef Graphics_h
#define Graphics_h

#include "Arduino.h"

class Graphics
{
  public:
    Graphics();
    void begin();
    void show();
    void clear();
    void drawDiagonal(byte absoluteX1, byte absoluteY1, byte relativeX2, float relativeY2, byte width, bool alignRight);
    void drawQuarterCircle(byte x, byte y, byte width, byte direction);
    void drawOuterQuarterCircle(byte x, byte y, byte width, byte direction);
    void drawSquare(byte x, byte y, byte xwidth, byte yheight);
    int drawText(long time, byte x, byte y, String text, byte fsize);
    int drawChar(long time, byte x, byte y, char c, byte fsize);
  private:
    int getLocalTime(long time);
    int roundUp(double a);
    int roundDown(double a);
};

#endif
