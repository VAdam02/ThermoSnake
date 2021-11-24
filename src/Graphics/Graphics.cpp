#include <Arduino.h>
#include "Graphics.h"

#define FONTSTEPSBYSIDE 60
#define FONTTIMEBETWEENSTEPS 100 //4 * FONTSTEPSBYSIDE * FONTTIMEBETWEENSTEPS = time needed for a round

#define PAGESWITCHTIME 2048 //x * SCREEN_WIDTH

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino NANO:       A4(SDA), A5(SCL)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Graphics::Graphics() { }

void Graphics::begin()
{
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever 
  }
  clear();
  show();
}

void Graphics::show()
{
  display.display();
}
void Graphics::clear()
{
  display.clearDisplay();
}

unsigned int Graphics::getLocalTime(unsigned long time)
{
  return (unsigned int)((time/FONTTIMEBETWEENSTEPS) % (4*FONTSTEPSBYSIDE));
}

void Graphics::drawDiagonal(byte x, byte y, byte width, byte dir)
{
  drawSquare(x, y, width, width);

  if (dir == 0) //nope
  {
    drawSquare(x+width, y-width, width, width);
    for (int i = 0; i < width; i++)
    {
      drawSquare(x+width-i, y-width+i, i, 1);
      drawSquare(x+width, y+i, width-1-i, 1);
    }
  }
  else if (dir == 1)
  {
    drawSquare(x+width, y+width, width, width);
    for (int i = 0; i < width; i++)
    {
      drawSquare(x+i+1, y+width+i, width-1-i, 1);
      drawSquare(x+width, y+i, i, 1);
    }
  }
  else if (dir == 2)
  {
    drawSquare(x-width, y+width, width, width);
    for (int i = 0; i < width; i++)
    {
      drawSquare(x-i, y+i, i, 1);
      drawSquare(x, y+width+i, width-1-i, 1);
    }
  }
  else if (dir == 3)
  {
    drawSquare(x-width, y-width, width, width);
    for (int i = 0; i < width; i++)
    {
      drawSquare(x, y-width+i, i, 1);
      drawSquare(x-width+i+1, y+i, width-1-i, 1);
    }
  }
}

void Graphics::drawQuarterCircle(byte x, byte y, byte width, int direction)
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
void Graphics::drawOuterQuarterCircle(byte x, byte y, byte width, int direction)
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

void Graphics::setPage(byte page)
{
  lastTime = (unsigned int)(millis() % 65536);
  targetPage = page;
  transitionX = 0;
}
byte Graphics::getCurPage()
{
  return curPage;
}
byte Graphics::getTargetPage()
{
  return targetPage;
}

void Graphics::refresh()
{
  if (curPage != targetPage)
  {
    unsigned int deltatime = (unsigned int)(millis() % 65536) - lastTime;

    transitionX = (unsigned long)SCREEN_WIDTH * deltatime / PAGESWITCHTIME;

    if (deltatime > PAGESWITCHTIME)
    {
      if (curPage > targetPage) { curPage--; }
      else { curPage++; }
      lastTime = (unsigned int)(millis() % 65536);
      transitionX = 0;
    }
  }
}

int Graphics::getCurXByPageX(byte page, byte x)
{
  int curx = (page * SCREEN_WIDTH) - (curPage * SCREEN_WIDTH) + x;

  if (curPage < targetPage) { curx -= transitionX; }
  else if (curPage > targetPage) { curx += transitionX; }

  if (curx > SCREEN_WIDTH)
  {
    return 255;
  }

  return curx;
}

int Graphics::drawText(byte page, byte x, byte y, String text, byte fsize)
{
  text.toUpperCase();
  for (int i = 0; i < text.length(); i++)
  {
    x += drawChar(page, x, y, text[i], fsize);
  }
  return x;
}
int Graphics::drawChar(byte page, byte x_, byte y_, char c, byte fsize)
{
  int x = x_;
  int y = y_;

  byte side = getLocalTime(millis()) / FONTSTEPSBYSIDE;
  byte step = round((double)(getLocalTime(millis())-side * FONTSTEPSBYSIDE) * fsize / FONTSTEPSBYSIDE);

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
  
  x = getCurXByPageX(page, x);

  if (x > SCREEN_WIDTH) { return 0; }
  if (x < -SCREEN_WIDTH) { return 0; }

  byte chars = EEPROM.read(2);
  byte i = 0;
  while (i < chars && !(c == EEPROM.read(3+i))) { i++; }
  if (!(i < chars))
  {
    drawSquare(x, y, 5*fsize, 5*fsize);
    return 6*fsize;
  }

  unsigned int startAddress = 3 + chars + (i*13);
  byte cache = EEPROM.read(startAddress);
  byte width = (byte)(cache >> 4);

  byte curNum = 0;
  for (i = 1; i < width * 5 + 1; i++)
  {
    if (i % 2 == 0) { cache = EEPROM.read(startAddress+(i/2)); }

    curNum = (i%2==0 ? (byte)(cache >> 4) : (byte)(cache & 0xF));
    switch (curNum)
    {
    case 1:
      drawSquare(x + ((i-1)/5)*fsize, y + ((i-1)%5)*fsize, fsize, fsize);
      break;

    case 2:
      drawQuarterCircle(x + ((i-1)/5)*fsize, y + ((i-1)%5)*fsize, fsize, 0);
      break;
    case 3:
      drawQuarterCircle(x + ((i-1)/5)*fsize, y + ((i-1)%5)*fsize, fsize, 0);
      drawOuterQuarterCircle(x + ((i-1)/5-1)*fsize, y + ((i-1)%5+1)*fsize, fsize, 0);
      break;

    case 4:
      drawQuarterCircle(x + ((i-1)/5)*fsize, y + ((i-1)%5)*fsize, fsize, 1);
      break;
    case 5:
      drawQuarterCircle(x + ((i-1)/5)*fsize, y + ((i-1)%5)*fsize, fsize, 1);
      drawOuterQuarterCircle(x + ((i-1)/5-1)*fsize, y + ((i-1)%5-1)*fsize, fsize, 1);
      break;

    case 6:
      drawQuarterCircle(x + ((i-1)/5)*fsize, y + ((i-1)%5)*fsize, fsize, 2);
      break;
    case 7:
      drawQuarterCircle(x + ((i-1)/5)*fsize, y + ((i-1)%5)*fsize, fsize, 2);
      drawOuterQuarterCircle(x + ((i-1)/5+1)*fsize, y + ((i-1)%5-1)*fsize, fsize, 2);
      break;

    case 8:
      drawQuarterCircle(x + ((i-1)/5)*fsize, y + ((i-1)%5)*fsize, fsize, 3);
      break;
    case 9:
      drawQuarterCircle(x + ((i-1)/5)*fsize, y + ((i-1)%5)*fsize, fsize, 3);
      drawOuterQuarterCircle(x + ((i-1)/5+1)*fsize, y + ((i-1)%5+1)*fsize, fsize, 3);
      break;

    case 10:
      drawDiagonal(x + ((i-1)/5)*fsize, y + ((i-1)%5)*fsize, fsize, 3);
      break;
    case 11:
      drawDiagonal(x + ((i-1)/5)*fsize, y + ((i-1)%5)*fsize, fsize, 2);
      break;

    case 12:
      drawQuarterCircle(x + ((i-1)/5)*fsize, y + ((i-1)%5)*fsize, fsize, 1);
      drawOuterQuarterCircle(x + ((i-1)/5-1)*fsize, y + ((i-1)%5-1)*fsize, fsize, 1);
      drawQuarterCircle(x + ((i-1)/5)*fsize, y + ((i-1)%5)*fsize, fsize, 3);
      drawOuterQuarterCircle(x + ((i-1)/5+1)*fsize, y + ((i-1)%5+1)*fsize, fsize, 3);
      break;
    case 13:
      if (fsize > 1) { drawQuarterCircle(x + ((i-1)/5)*fsize, y + ((i-1)%5)*fsize, fsize, 0); }
      else { drawSquare(x + ((i-1)/5)*fsize, y + ((i-1)%5)*fsize, fsize, fsize); }
      break;

    case 14:
      drawDiagonal(x + ((i-1)/5)*fsize, y + ((i-1)%5)*fsize, fsize, 1);
      break;
    case 15:
      drawDiagonal(x + ((i-1)/5)*fsize, y + ((i-1)%5)*fsize, fsize, 0);
      break;
    }
  }

  return (width+1)*fsize;
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
