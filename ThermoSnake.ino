#include "src/Graphics/Graphics.h"

Graphics oled;

long time = 1;

void setup()
{
  oled.begin();

  noTone(4);
  
  int fsize = 1;
  oled.drawText(time, 0, 1, "abcdefghijklm", fsize);
  oled.drawText(time, 0, 1+1*6*fsize, "nopqrstuvwxyz", fsize);
  oled.drawText(time, 0, 1+2*6*fsize, "0123456789.*%", fsize);
  oled.drawText(time, 0, 1+3*6*fsize, ": ", fsize);
  oled.show();
  
  delay(3000);
}

void loop()
{
  oled.clear();

  int a = 0;
  char c = '5';
  oled.drawChar(time, a, 1, c, 6);
  a = 6 * 6 + a;
  oled.drawChar(time, a, 1, c, 5);
  a = 6 * 5 + a;
  oled.drawChar(time, a, 1, c, 4);
  a = 6 * 4 + a;
  oled.drawChar(time, a, 1, c, 3);
  a = 6 * 3 + a;
  oled.drawChar(time, a, 1, c, 2);
  a = 6 * 2 + a;
  oled.drawChar(time, a, 1, c, 1);
  
  oled.show();

  delay(50);
  time += 50;
}
