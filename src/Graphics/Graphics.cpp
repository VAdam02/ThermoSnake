#include "Graphics.h"
#include <Arduino.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino NANO:       A4(SDA), A5(SCL)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Graphics::Graphics() { }

void Graphics::begin()
{
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever 
  }
  display.clearDisplay();
}

void Graphics::show()
{
  display.display();
}
void Graphics::clear()
{
  display.clearDisplay();
}

void Graphics::drawDiagonal(byte absoluteX1, byte absoluteY1, byte relativeX2, float relativeY2, byte width, bool alignRight)
{
  for (int j = 0; abs(j) < abs(relativeY2*width)-width; j+=(relativeY2/abs(relativeY2))) //-y
  for (int i = 0; i < width; i++) //x - width
  if (alignRight) { display.drawPixel(absoluteX1 +roundUp(j*relativeX2/relativeY2) + i, absoluteY1 -(j), WHITE); }
  else { display.drawPixel(absoluteX1 +roundDown(j*relativeX2/relativeY2) + i, absoluteY1 -(j), WHITE); }
}

void Graphics::drawQuarterCircle(byte x, byte y, byte width, byte direction)
{
  for (int i = 1; i <= width; i++)
  for (int j = 1; j <= width; j++)
  if ((i*i)+(j*j) <= (width*width))
  {
    if (direction % 4 == 0) { display.drawPixel(x +(i-1), y +(width-j), WHITE); }
    else if (direction % 4 == 1) { display.drawPixel(x +(i-1), y +(j-1), WHITE); }
    else if (direction % 4 == 2) { display.drawPixel(x +(width-i), y +(j-1), WHITE); }
    else { display.drawPixel(x +(width-i), y +(width-j), WHITE); }
  }
}
void Graphics::drawOuterQuarterCircle(byte x, byte y, byte width, byte direction)
{
  for (int i = 1; i <= (width/2); i++)
  for (int j = 1; j <= (width/2); j++)
  if ((i*i)+(j*j) >= (width/2*width/2))
  {
    if (direction % 4 == 0) { display.drawPixel(x +(i-1) + width - (width/2), y +(width/2-j), WHITE); }
    else if (direction % 4 == 1) { display.drawPixel(x +(i-1) + width - (width/2), y +(j-1) + width - (width/2), WHITE); }
    else if (direction % 4 == 2) { display.drawPixel(x +(width/2-i), y +(j-1) +width -(width/2), WHITE); }
    else { display.drawPixel(x +(width/2-i), y +(width/2-j), WHITE); }
  }
}

void Graphics::drawSquare(byte x, byte y, byte xwidth, byte yheight)
{
  for (int i = 0; i < xwidth; i++)
  for (int j = 0; j < yheight; j++)
  display.drawPixel(x+i, y+j, WHITE);
}

void Graphics::drawText(int time, byte x, byte y, String text, byte fsize)
{
  for (int i = 0; i < text.length(); i++)
  {
    drawChar(time, x + (i*fsize*6), y, text[i], fsize);
  }
}
void Graphics::drawChar(int time, byte x, byte y, char c, byte fsize)
{
  time = time % (60*4);

  byte side = time / 60;
  byte step = round((double)(time-side * 60) * fsize /60);

  if (side == 0)
  {
    x += step;
  }
  else if (side == 1)
  {
    x += fsize;
    y += step;
  }
  else if (side == 2)
  {
    x += (fsize) - step;
    y += fsize;
  }
  else
  {
    y += (fsize) - step;
  }
  
  drawSquare(x, y, 5*fsize, 5*fsize);
}

int  Graphics::roundUp(double a)
{
  int b = a;
  if (a == b)
  {
    return b;
  }
  else
  {
    return b+1;
  }
}

int Graphics::roundDown(double a)
{
  return a;
}
