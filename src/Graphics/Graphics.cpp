#include "Graphics.h"
#include <Arduino.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

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

  display.dim(true);
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

byte curPage = 0;
byte targetPage = 0;
byte transitionX = 0;
unsigned int G_lastTime = 0;

void Graphics::setPage(byte page)
{
  G_lastTime = (unsigned int)(millis() % 65536);
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
    unsigned int deltatime = (unsigned int)(millis() % 65536) - G_lastTime;

    transitionX = (unsigned long)SCREEN_WIDTH * deltatime / PAGESWITCHTIME;

    if (deltatime > PAGESWITCHTIME)
    {
      if (curPage > targetPage) { curPage--; }
      else { curPage++; }
      G_lastTime = (unsigned int)(millis() % 65536);
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

  if (c == 'A' || c == 'a')
  {
    drawSquare(x, y+fsize, fsize, 4*fsize); //Left pill
    drawSquare(x+4*fsize, y+fsize, fsize, 4*fsize); //Right pill
    drawSquare(x+fsize, y+2*fsize, 3*fsize, fsize); //Bottom line
    drawSquare(x+fsize, y, 3*fsize, fsize); //Top line
    drawQuarterCircle(x+4*fsize, y, fsize, 0); //TopRight  round
    drawOuterQuarterCircle(x+3*fsize, y+fsize, fsize, 0);
    drawQuarterCircle(x, y, fsize, 3); //TopLeft  round
    drawOuterQuarterCircle(x+fsize, y+fsize, fsize, 3);
  }
  else if (c == 'B' || c == 'b')
  {
    drawSquare(x, y, fsize, 5*fsize); //Left pill
    drawSquare(x+1*fsize, y, 3*fsize, fsize); //Top line
    drawQuarterCircle(x+4*fsize, y, fsize, 0); //TopRight  round
    drawOuterQuarterCircle(x+3*fsize, y+fsize, fsize, 0);
    drawSquare(x+4*fsize, y+1*fsize, fsize, fsize); //Right top pill
    drawQuarterCircle(x+4*fsize, y+2*fsize, fsize, 1); //Middle round
    drawOuterQuarterCircle(x+3*fsize, y+fsize, fsize, 1);
    drawSquare(x+1*fsize, y+2*fsize, 3*fsize, fsize); //Middle line
    drawQuarterCircle(x+4*fsize, y+2*fsize, fsize, 0); //Middle round 2
    drawOuterQuarterCircle(x+3*fsize, y+3*fsize, fsize, 0);
    drawSquare(x+4*fsize, y+3*fsize, fsize, fsize); //Right bottom pill
    drawQuarterCircle(x+4*fsize, y+4*fsize, fsize, 1); //Bottom round
    drawOuterQuarterCircle(x+3*fsize, y+3*fsize, fsize, 1);
    drawSquare(x+1*fsize, y+4*fsize, 3*fsize, fsize); //Bottom line
  }
  else if (c == 'C' || c == 'c')
  {
    drawSquare(x+4*fsize, y+1*fsize, fsize, fsize); //RightTop pill
    drawQuarterCircle(x+4*fsize, y, fsize, 0); //TopRight  round
    drawOuterQuarterCircle(x+3*fsize, y+fsize, fsize, 0);
    drawSquare(x+1*fsize, y, 3*fsize, fsize); //Top line
    drawQuarterCircle(x, y, fsize, 3); //TopLeft  round
    drawOuterQuarterCircle(x+fsize, y+fsize, fsize, 3);
    drawSquare(x, y+fsize, fsize, 3*fsize); //Left pill
    drawQuarterCircle(x, y+4*fsize, fsize, 2); //BottomLeft  round
    drawOuterQuarterCircle(x+fsize, y+3*fsize, fsize, 2);
    drawSquare(x+1*fsize, y+4*fsize, 3*fsize, fsize); //Bottom line
    drawQuarterCircle(x+4*fsize, y+4*fsize, fsize, 1); //Bottom round
    drawOuterQuarterCircle(x+3*fsize, y+3*fsize, fsize, 1);
    drawSquare(x+4*fsize, y+3*fsize, fsize, fsize); //RightBottom pill
  }
  else if (c == 'D' || c == 'd')
  {
    drawSquare(x, y, fsize, 5*fsize); //Left pill
    drawSquare(x+1*fsize, y, 3*fsize, fsize); //Top line
    drawQuarterCircle(x+4*fsize, y, fsize, 0); //TopRight  round
    drawOuterQuarterCircle(x+3*fsize, y+fsize, fsize, 0);
    drawSquare(x+4*fsize, y+fsize, fsize, 3*fsize); //Right pill
    drawQuarterCircle(x+4*fsize, y+4*fsize, fsize, 1); //Bottom round
    drawOuterQuarterCircle(x+3*fsize, y+3*fsize, fsize, 1);
    drawSquare(x+1*fsize, y+4*fsize, 3*fsize, fsize); //Bottom line
  }
  else if (c == 'E' || c == 'e')
  {
    drawSquare(x, y, fsize, 5*fsize); //Left pill
    drawSquare(x+1*fsize, y, 4*fsize, fsize); //Top line
    drawSquare(x+1*fsize, y+2*fsize, 4*fsize, fsize); //Middle line
    drawSquare(x+1*fsize, y+4*fsize, 4*fsize, fsize); //Bottom line
  }
  else if (c == 'F' || c == 'f')
  {
    drawSquare(x, y, fsize, 5*fsize); //Left pill
    drawSquare(x+1*fsize, y, 4*fsize, fsize); //Top line
    drawSquare(x+1*fsize, y+2*fsize, 3*fsize, fsize); //Middle line
  }
  else if (c == 'G' || c == 'g')
  {
    
    if (fsize > 1) { drawQuarterCircle(x+4*fsize, y, fsize, 0); } //TopRight  round
    else { drawSquare(x+4*fsize, y, fsize, fsize); } //Top line
    
    drawSquare(x+1*fsize, y, 3*fsize, fsize); //Top line
    drawQuarterCircle(x, y, fsize, 3); //TopLeft  round
    drawOuterQuarterCircle(x+fsize, y+fsize, fsize, 3);
    drawSquare(x, y+fsize, fsize, 3*fsize); //Left pill
    drawQuarterCircle(x, y+4*fsize, fsize, 2); //BottomLeft  round
    drawOuterQuarterCircle(x+fsize, y+3*fsize, fsize, 2);
    drawSquare(x+1*fsize, y+4*fsize, 3*fsize, fsize); //Bottom line
    drawQuarterCircle(x+4*fsize, y+4*fsize, fsize, 1); //Bottom round
    drawOuterQuarterCircle(x+3*fsize, y+3*fsize, fsize, 1);
    drawSquare(x+4*fsize, y+2*fsize, fsize, 2*fsize); //RightBottom pill
    drawSquare(x+3*fsize, y+2*fsize, fsize, fsize); //Middle line
  }
  else if (c == 'H' || c == 'h')
  {
    drawSquare(x, y, fsize, 5*fsize); //Left pill
    drawSquare(x+fsize, y+2*fsize, 3*fsize, fsize); //MiddleLine
    drawSquare(x+4*fsize, y, fsize, 5*fsize); //Right pill
  }
  else if (c == 'I' || c == 'i')
  {
    drawSquare(x+fsize, y, 3*fsize, fsize); //Top Line
    drawSquare(x+2*fsize, y+fsize, fsize, 3*fsize); //Middle pill
    drawSquare(x+fsize, y+4*fsize, 3*fsize, fsize); //Bottom Line
  }
  else if (c == 'J' || c == 'j')
  {
    drawSquare(x, y, 5*fsize, fsize); //Top line
    drawSquare(x+4*fsize, y+1*fsize, fsize, 3*fsize); //Right pill
    drawQuarterCircle(x+4*fsize, y+4*fsize, fsize, 1); //BottomRight  round
    drawOuterQuarterCircle(x+3*fsize, y+3*fsize, fsize, 1);
    drawSquare(x+fsize, y+4*fsize, 3*fsize, fsize); //Bottom line
    drawQuarterCircle(x, y+4*fsize, fsize, 2); //BottomLeft  round
    drawOuterQuarterCircle(x+fsize, y+3*fsize, fsize, 2);
    drawSquare(x, y+3*fsize, fsize, fsize); //Right pill
  }
  else if (c == 'K' || c == 'k') //TODO
  {
    drawSquare(x, y, fsize, 5*fsize); //Left pill
    drawSquare(x+fsize, y+2*fsize, fsize, fsize); //Middle line
    //drawDiagonal(x+fsize, y+3*fsize-1, 4, 4, fsize, true); //Top pill
    //drawDiagonal(x+fsize, y+2*fsize, 4, -4, fsize, true); //Bottom pill
  }
  else if (c == 'L' || c == 'l')
  {
    drawSquare(x, y, fsize, 5*fsize); //Left pill
    drawSquare(x+fsize, y+4*fsize, 3*fsize, fsize); //Bottom line
  }
  else if (c == 'M' || c == 'm') //TODO
  {
    drawSquare(x, y, fsize, 5*fsize); //Left pill
    //drawDiagonal(x, y, 3, -3, fsize, true); //Left line
    //drawDiagonal(x+2*fsize, y+2*fsize, 3, 3, fsize, true); //Right line
    drawSquare(x+4*fsize, y, fsize, 5*fsize); //Right pill
  }
  else if (c == 'N' || c == 'n') //TODO
  {
    drawSquare(x, y, fsize, 5*fsize); //Left pill
    //drawDiagonal(x, y, 5, -6, fsize, true); //Left line
    drawSquare(x+4*fsize, y, fsize, 5*fsize); //Right pill
  }
  else if (c == 'O' || c == 'o')
  {
    drawQuarterCircle(x+4*fsize, y, fsize, 0); //TopRight  round
    drawOuterQuarterCircle(x+3*fsize, y+fsize, fsize, 0);
    drawSquare(x+1*fsize, y, 3*fsize, fsize); //Top line
    drawQuarterCircle(x, y, fsize, 3); //TopLeft  round
    drawOuterQuarterCircle(x+fsize, y+fsize, fsize, 3);
    drawSquare(x, y+fsize, fsize, 3*fsize); //Left pill
    drawQuarterCircle(x, y+4*fsize, fsize, 2); //BottomLeft  round
    drawOuterQuarterCircle(x+fsize, y+3*fsize, fsize, 2);
    drawSquare(x+1*fsize, y+4*fsize, 3*fsize, fsize); //Bottom line
    drawQuarterCircle(x+4*fsize, y+4*fsize, fsize, 1); //Bottom round
    drawOuterQuarterCircle(x+3*fsize, y+3*fsize, fsize, 1);
    drawSquare(x+4*fsize, y+fsize, fsize, 3*fsize); //Right pill
  }
  else if (c == 'P' || c == 'p')
  {
    drawSquare(x, y, fsize, 5*fsize); //Left pill
    drawSquare(x+fsize, y, 3*fsize, fsize); //Top line
    drawQuarterCircle(x+4*fsize, y, fsize, 0); //Top round
    drawOuterQuarterCircle(x+3*fsize, y+fsize, fsize, 0);
    drawSquare(x+4*fsize, y+fsize, fsize, fsize); //Right pill
    drawQuarterCircle(x+4*fsize, y+2*fsize, fsize, 1); //Bottom round
    drawOuterQuarterCircle(x+3*fsize, y+1*fsize, fsize, 1);
    drawSquare(x+fsize, y+2*fsize, 3*fsize, fsize); //Bottom line
  }
  else if (c == 'Q' || c == 'q')
  {
    drawQuarterCircle(x+4*fsize, y, fsize, 0); //TopRight  round
    drawOuterQuarterCircle(x+3*fsize, y+fsize, fsize, 0);
    drawSquare(x+1*fsize, y, 3*fsize, fsize); //Top line
    drawQuarterCircle(x, y, fsize, 3); //TopLeft  round
    drawOuterQuarterCircle(x+fsize, y+fsize, fsize, 3);
    drawSquare(x, y+fsize, fsize, 3*fsize); //Left pill
    drawQuarterCircle(x, y+4*fsize, fsize, 2); //BottomLeft  round
    drawOuterQuarterCircle(x+fsize, y+3*fsize, fsize, 2);
    drawSquare(x+1*fsize, y+4*fsize, 3*fsize, fsize); //Bottom line
    drawQuarterCircle(x+4*fsize, y+4*fsize, fsize, 1); //Bottom round
    drawOuterQuarterCircle(x+3*fsize, y+3*fsize, fsize, 1);
    drawSquare(x+4*fsize, y+fsize, fsize, 3*fsize); //RightBottom pill
    drawDiagonal(x+3*fsize, y+3*fsize, 2, -2, fsize, true); //Bottom pill
  }
  else if (c == 'R' || c == 'r') //TODO
  {
    drawSquare(x, y, fsize, 5*fsize); //Left pill
    drawSquare(x+fsize, y, 3*fsize, fsize); //Top line
    drawQuarterCircle(x+4*fsize, y, fsize, 0); //Top round
    drawOuterQuarterCircle(x+3*fsize, y+fsize, fsize, 0);
    drawSquare(x+4*fsize, y+fsize, fsize, fsize); //Right pill
    drawQuarterCircle(x+4*fsize, y+2*fsize, fsize, 1); //Bottom round
    drawOuterQuarterCircle(x+3*fsize, y+1*fsize, fsize, 1);
    drawSquare(x+fsize, y+2*fsize, 3*fsize, fsize); //Bottom line
    //drawDiagonal(x+3*fsize, y+2*fsize, 1, -4, fsize, true); //Bottom pill
  }
  else if (c == 'S' || c == 's')
  {
    drawSquare(x+1*fsize, y, 4*fsize, fsize); //Top line
    drawQuarterCircle(x, y, fsize, 3); //TopLeft  round
    drawOuterQuarterCircle(x+fsize, y+fsize, fsize, 3);
    drawSquare(x, y+fsize, fsize, fsize); //Left pill
    drawQuarterCircle(x, y+2*fsize, fsize, 2); //BottomLeft  round
    drawOuterQuarterCircle(x+fsize, y+fsize, fsize, 2);
    drawSquare(x+1*fsize, y+2*fsize, 3*fsize, fsize); //Middle line
    drawQuarterCircle(x+4*fsize, y+2*fsize, fsize, 0); //TopRight  round
    drawOuterQuarterCircle(x+3*fsize, y+3*fsize, fsize, 0);
    drawSquare(x+4*fsize, y+3*fsize, fsize, fsize); //Right pill
    drawQuarterCircle(x+4*fsize, y+4*fsize, fsize, 1); //Bottom round
    drawOuterQuarterCircle(x+3*fsize, y+3*fsize, fsize, 1);
    drawSquare(x, y+4*fsize, 4*fsize, fsize); //Bottom line
  }
  else if (c == 'T' || c == 't')
  {
    drawSquare(x+2*fsize, y+fsize, fsize, 4*fsize); //Left pill
    drawSquare(x, y, 5*fsize, fsize); //Top line
  }
  else if (c == 'U' || c == 'u')
  {
    drawSquare(x, y, fsize, 4*fsize); //Left pill
    drawQuarterCircle(x, y+4*fsize, fsize, 2); //BottomLeft  round
    drawOuterQuarterCircle(x+fsize, y+3*fsize, fsize, 2);
    drawSquare(x+1*fsize, y+4*fsize, 3*fsize, fsize); //Bottom line
    drawQuarterCircle(x+4*fsize, y+4*fsize, fsize, 1); //BottomRight round
    drawOuterQuarterCircle(x+3*fsize, y+3*fsize, fsize, 1);
    drawSquare(x+4*fsize, y, fsize, 4*fsize); //Right pill
  }
  else if (c == 'V' || c == 'v') //TODO
  {
    drawSquare(x, y, fsize, 3*fsize); //Left pill
    drawSquare(x+4*fsize, y, fsize, 3*fsize); //Right pill
  }
  else if (c == 'W' || c == 'w')
  {
    drawSquare(x, y, fsize, 4*fsize); //Left pill
    drawQuarterCircle(x, y+4*fsize, fsize, 2); //BottomLeft  round
    drawOuterQuarterCircle(x+fsize, y+3*fsize, fsize, 2);
    drawSquare(x+1*fsize, y+4*fsize, fsize, fsize); //Bottom line
    drawQuarterCircle(x+2*fsize, y+4*fsize, fsize, 1); //MiddleLeft  round
    drawOuterQuarterCircle(x+fsize, y+3*fsize, fsize, 1);
    
    drawSquare(x+2*fsize, y, fsize, 4*fsize); //Middle pill
    
    drawQuarterCircle(x+2*fsize, y+4*fsize, fsize, 2); //MiddleRight  round
    drawOuterQuarterCircle(x+3*fsize, y+3*fsize, fsize, 2);
    drawSquare(x+3*fsize, y+4*fsize, fsize, fsize); //Bottom line2
    drawQuarterCircle(x+4*fsize, y+4*fsize, fsize, 1); //Bottom round
    drawOuterQuarterCircle(x+3*fsize, y+3*fsize, fsize, 1);
    drawSquare(x+4*fsize, y, fsize, 4*fsize); //Right pill
  }
  else if (c == 'X' || c == 'x') //TODO
  {
    
  }
  else if (c == 'Y' || c == 'y') //TODO
  {
    drawSquare(x+2*fsize, y+2*fsize, fsize, 3*fsize); //Left pill
  }
  else if (c == 'Z' || c == 'z') //TODO
  {
    drawSquare(x, y, 5*fsize, fsize); //Top line
    drawSquare(x, y+4*fsize, 5*fsize, fsize); //Bottom line
  }
  else if (c == '0') //TODO cross trough the circle
  {
    drawQuarterCircle(x+4*fsize, y, fsize, 0); //TopRight  round
    drawOuterQuarterCircle(x+3*fsize, y+fsize, fsize, 0);
    drawSquare(x+1*fsize, y, 3*fsize, fsize); //Top line
    drawQuarterCircle(x, y, fsize, 3); //TopLeft  round
    drawOuterQuarterCircle(x+fsize, y+fsize, fsize, 3);
    drawSquare(x, y+fsize, fsize, 3*fsize); //Left pill
    drawQuarterCircle(x, y+4*fsize, fsize, 2); //BottomLeft  round
    drawOuterQuarterCircle(x+fsize, y+3*fsize, fsize, 2);
    drawSquare(x+1*fsize, y+4*fsize, 3*fsize, fsize); //Bottom line
    drawQuarterCircle(x+4*fsize, y+4*fsize, fsize, 1); //Bottom round
    drawOuterQuarterCircle(x+3*fsize, y+3*fsize, fsize, 1);
    drawSquare(x+4*fsize, y+fsize, fsize, 3*fsize); //Right pill
  }
  else if (c == '1')
  {
    drawSquare(x+fsize, y+4*fsize, 3*fsize, fsize); //Bottom line
    drawSquare(x+2*fsize, y, fsize, 4*fsize); //Middle pill
    drawSquare(x+fsize, y+fsize, fsize, fsize); //Left line
  }
  else if (c == '2')
  {
    drawSquare(x, y, 4*fsize, fsize); //Top line
    drawQuarterCircle(x+4*fsize, y, fsize, 0); //TopRight  round
    drawOuterQuarterCircle(x+3*fsize, y+fsize, fsize, 0);
    drawSquare(x+4*fsize, y+fsize, fsize, fsize); //Right pill
    drawQuarterCircle(x+4*fsize, y+2*fsize, fsize, 1); //Bottom round
    drawOuterQuarterCircle(x+3*fsize, y+fsize, fsize, 1);
    drawSquare(x+1*fsize, y+2*fsize, 3*fsize, fsize); //Middle line
    drawQuarterCircle(x, y+2*fsize, fsize, 3); //TopLeft  round
    drawOuterQuarterCircle(x+fsize, y+3*fsize, fsize, 3);
    drawSquare(x, y+3*fsize, fsize, fsize); //Left pill
    drawSquare(x, y+4*fsize, 5*fsize, fsize); //Bottom line
  }
  else if (c == '3')
  {
    if (fsize > 1) { drawQuarterCircle(x, y, fsize, 3); }
    else { drawSquare(x, y, fsize, fsize); }
    drawSquare(x+fsize, y, 3*fsize, fsize); //Top line
    drawQuarterCircle(x+4*fsize, y, fsize, 0); //TopRight  round
    drawOuterQuarterCircle(x+3*fsize, y+fsize, fsize, 0);
    drawSquare(x+4*fsize, y+fsize, fsize, fsize); //Right pill
    drawQuarterCircle(x+4*fsize, y+2*fsize, fsize, 1); //Bottom round
    drawOuterQuarterCircle(x+3*fsize, y+fsize, fsize, 1);
    drawSquare(x+fsize, y+2*fsize, 3*fsize, fsize); //Middle line
    drawQuarterCircle(x+4*fsize, y+2*fsize, fsize, 0); //TopRight  round2
    drawOuterQuarterCircle(x+3*fsize, y+3*fsize, fsize, 0);
    drawSquare(x+4*fsize, y+3*fsize, fsize, fsize); //Right pill2
    drawQuarterCircle(x+4*fsize, y+4*fsize, fsize, 1); //Bottom round2
    drawOuterQuarterCircle(x+3*fsize, y+3*fsize, fsize, 1);
    drawSquare(x+fsize, y+4*fsize, 3*fsize, fsize); //Bottom line
    if (fsize > 1) { drawQuarterCircle(x, y+4*fsize, fsize, 2); }
    else { drawSquare(x, y+4*fsize, fsize, fsize); }
  }
  else if (c == '4')
  {
    drawSquare(x, y, fsize, 2*fsize); //Left pill
    drawQuarterCircle(x, y+2*fsize, fsize, 2); //BottomLeft  round
    drawOuterQuarterCircle(x+fsize, y+1*fsize, fsize, 2);
    drawSquare(x+fsize, y+2*fsize, 3*fsize, fsize); //Bottom line
    drawSquare(x+4*fsize, y, fsize, 5*fsize); //Right pill
  }
  else if (c == '5')
  {
    drawSquare(x, y, 5*fsize, fsize); //Top line
    drawSquare(x, y+fsize, fsize, 2*fsize); //Left pill
    drawSquare(x+fsize, y+2*fsize, 3*fsize, fsize); //Middle line
    drawQuarterCircle(x+4*fsize, y+2*fsize, fsize, 0); //TopRight  round
    drawOuterQuarterCircle(x+3*fsize, y+3*fsize, fsize, 0);
    drawSquare(x+4*fsize, y+3*fsize, fsize, fsize); //Right pill
    drawQuarterCircle(x+4*fsize, y+4*fsize, fsize, 1); //BottomRight  round
    drawOuterQuarterCircle(x+3*fsize, y+3*fsize, fsize, 1);
    drawSquare(x, y+4*fsize, 4*fsize, fsize); //Bottom line
  }
  else if (c == '6')
  {
    drawQuarterCircle(x+4*fsize, y, fsize, 0); //TopRight  round
    drawSquare(x+fsize, y, 3*fsize, fsize); //Top line
    drawQuarterCircle(x, y, fsize, 3); //TopLeft  round
    drawOuterQuarterCircle(x+fsize, y+fsize, fsize, 3);
    drawSquare(x, y+fsize, fsize, 3*fsize); //Left pill
    drawQuarterCircle(x, y+4*fsize, fsize, 2); //BottomLeft  round
    drawOuterQuarterCircle(x+fsize, y+3*fsize, fsize, 2);
    drawSquare(x+fsize, y+4*fsize, 3*fsize, fsize); //Bottom line
    drawQuarterCircle(x+4*fsize, y+4*fsize, fsize, 1); //BottomRight  round
    drawOuterQuarterCircle(x+3*fsize, y+3*fsize, fsize, 1);
    drawSquare(x+4*fsize, y+3*fsize, fsize, fsize); //Right pill
    drawQuarterCircle(x+4*fsize, y+2*fsize, fsize, 0); //TopRight  round
    drawOuterQuarterCircle(x+3*fsize, y+3*fsize, fsize, 0);
    drawSquare(x+fsize, y+2*fsize, 3*fsize, fsize); //Middle line
  }
  else if (c == '7')
  {
    drawSquare(x+fsize, y, 3*fsize, fsize); //Top line
    drawSquare(x+4*fsize, y, fsize, 5*fsize); //Right pill
  }
  else if (c == '8')
  {
    drawQuarterCircle(x+3*fsize, y+2*fsize, fsize, 1); //MiddleBottomRight round
    drawOuterQuarterCircle(x+2*fsize, y+fsize, fsize, 1);
    drawSquare(x+3*fsize, y+fsize, fsize, fsize); //TopRight pill
    drawQuarterCircle(x+3*fsize, y, fsize, 0); //TopRight  round
    drawOuterQuarterCircle(x+2*fsize, y+fsize, fsize, 0);
    drawSquare(x+fsize, y, 2*fsize, fsize); //Top line
    drawQuarterCircle(x, y, fsize, 3); //TopLeft  round
    drawOuterQuarterCircle(x+fsize, y+fsize, fsize, 3);
    drawSquare(x, y+fsize, fsize, fsize); //TopLeft pill
    drawQuarterCircle(x, y+2*fsize, fsize, 2); //MiddleBottomLeft  round
    drawOuterQuarterCircle(x+fsize, y+fsize, fsize, 2);
    drawSquare(x+1*fsize, y+2*fsize, 2*fsize, fsize); //Middle line
    drawQuarterCircle(x, y+2*fsize, fsize, 3); //MiddleTopLeft  round
    drawOuterQuarterCircle(x+fsize, y+3*fsize, fsize, 3);
    drawSquare(x, y+3*fsize, fsize, fsize); //BottomLeft pill
    drawQuarterCircle(x, y+4*fsize, fsize, 2); //BottomLeft  round
    drawOuterQuarterCircle(x+fsize, y+3*fsize, fsize, 2);
    drawSquare(x+1*fsize, y+4*fsize, 2*fsize, fsize); //Bottom line
    drawQuarterCircle(x+3*fsize, y+4*fsize, fsize, 1); //BottomRight round
    drawOuterQuarterCircle(x+2*fsize, y+3*fsize, fsize, 1);
    drawSquare(x+3*fsize, y+3*fsize, fsize, fsize); //BottomRight pill
    drawQuarterCircle(x+3*fsize, y+2*fsize, fsize, 0); //MiddleTopRight  round
    drawOuterQuarterCircle(x+2*fsize, y+3*fsize, fsize, 0);
  }
  else if (c == '9')
  {
    drawSquare(x+3*fsize, y+fsize, fsize, 3*fsize); //TopRight pill
    drawQuarterCircle(x+3*fsize, y, fsize, 0); //TopRight  round
    drawOuterQuarterCircle(x+2*fsize, y+fsize, fsize, 0);
    drawSquare(x+fsize, y, 2*fsize, fsize); //Top line
    drawQuarterCircle(x, y, fsize, 3); //TopLeft  round
    drawOuterQuarterCircle(x+fsize, y+fsize, fsize, 3);
    drawSquare(x, y+fsize, fsize, fsize); //TopLeft pill
    drawQuarterCircle(x, y+2*fsize, fsize, 2); //MiddleBottomLeft  round
    drawOuterQuarterCircle(x+fsize, y+fsize, fsize, 2);
    drawSquare(x+1*fsize, y+2*fsize, 2*fsize, fsize); //Middle line
    drawQuarterCircle(x, y+4*fsize, fsize, 2); //BottomLeft  round
    drawSquare(x+1*fsize, y+4*fsize, 2*fsize, fsize); //Bottom line
    drawQuarterCircle(x+3*fsize, y+4*fsize, fsize, 1); //BottomRight round
    drawOuterQuarterCircle(x+2*fsize, y+3*fsize, fsize, 1);
  }
  else if (c == '.')
  {
    drawSquare(x, y+4*fsize, fsize, fsize); //Dot
    return 2*fsize;
  }
  else if (c == '*')
  {
    drawQuarterCircle(x+2*fsize, y+2*fsize, fsize, 1); //MiddleBottomRight round
    drawSquare(x+2*fsize, y+fsize, fsize, fsize); //TopRight pill
    drawQuarterCircle(x+2*fsize, y, fsize, 0); //TopRight  round
    drawSquare(x+fsize, y, fsize, fsize); //Top line
    drawQuarterCircle(x, y, fsize, 3); //TopLeft  round
    drawSquare(x, y+fsize, fsize, fsize); //TopLeft pill
    drawQuarterCircle(x, y+2*fsize, fsize, 2); //MiddleBottomLeft  round
    drawSquare(x+1*fsize, y+2*fsize, fsize, fsize); //Middle line
    return 4*fsize;
  }
  else if (c == '%') //TODO
  {
    drawSquare(x, y, fsize, fsize); //Left dot
    drawSquare(x+4*fsize, y+4*fsize, fsize, fsize); //Right dot
  }
  else if (c == ':')
  {
    drawSquare(x, y+fsize, fsize, fsize); //Top dot
    drawSquare(x, y+3*fsize, fsize, fsize); //Bottom dot
    return 2*fsize;
  }
  else if (c == ' ')
  {
    
  }
  else
  {
    drawSquare(x, y, 5*fsize, 5*fsize);
  }

  return 6*fsize;
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
