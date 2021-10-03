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

void setup()
{
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.clearDisplay();
  display.display();
  /*
  display.setTextSize(4);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  */

  /*
  for (int x=0; x < 128; x++)
  {
    display.drawPixel(x, 0, SSD1306_WHITE);
  }

  for (int x=0; x < 128; x++)
  {
    display.drawPixel(x, 31, SSD1306_WHITE);
  }

  for (int y=0; y < 32; y++)
  {
    display.drawPixel(0, y, SSD1306_WHITE);
  }

  for (int y=0; y < 32; y++)
  {
    display.drawPixel(127, y, SSD1306_WHITE);
  }
  */
  
  int a = 0;
  char c = 'B';
  drawChar(a, 1, c, 6);
  a = 6 * 6 + a;
  drawChar(a, 1, c, 5);
  a = 6 * 5 + a;
  drawChar(a, 1, c, 4);
  a = 6 * 4 + a;
  drawChar(a, 1, c, 3);
  a = 6 * 3 + a;
  drawChar(a, 1, c, 2);
  a = 6 * 2 + a;
  drawChar(a, 1, c, 1);
  
  display.display();
}

void loop()
{
  
}

void drawDiagonal(byte absoluteX1, byte absoluteY1, byte relativeX2, float relativeY2, byte width, bool alignRight)
{  
  if (relativeY2 > 0) { absoluteY1--; }
  
  for (int j = 0; abs(j) < abs(relativeY2*width); j+=(relativeY2/abs(relativeY2))) //-y
  for (int i = 0; i < width; i++) //x - width
  if (alignRight) { display.drawPixel(absoluteX1 +roundUp(j*relativeX2/relativeY2) + i, absoluteY1 -(j), WHITE); }
  else { display.drawPixel(absoluteX1 +roundDown(j*relativeX2/relativeY2) + i, absoluteY1 -(j), WHITE); }
}

void drawQuarterCircle(byte x, byte y, byte width, byte direction)
{
  for (int i = 1; i <= width; i++)
  for (int j = 1; j <= width; j++)
  if ((i*i)+(j*j) <= (width*width))
  {
    if (direction % 4 == 0) { display.drawPixel(x +(i-1), y +(width-j), WHITE); }
    else if (direction % 4 == 1) { display.drawPixel(x +(i-1), y +(j-1), WHITE); }
    else if (direction % 4 == 2) { /*TODO*/ }
    else { /*TODO*/ }
  }
}
void drawOuterQuarterCircle(byte x, byte y, byte width, byte direction)
{
  for (int i = 1; i <= (width/2); i++)
  for (int j = 1; j <= (width/2); j++)
  if ((i*i)+(j*j) >= (width/2*width/2))
  {
    if (direction % 4 == 0) { display.drawPixel(x +(i-1) - (width/2), y +(width/2-j), WHITE); }
    else if (direction % 4 == 1) { display.drawPixel(x +(i-1) - (width/2), y +(-width/2+j-1), WHITE); }
    else if (direction % 4 == 2) { /*TODO*/ }
    else { /*TODO*/ }
  }
}

void drawSquare(byte x, byte y, byte xwidth, byte yheight)
{
  for (int i = 0; i < xwidth; i++)
  for (int j = 0; j < yheight; j++)
  display.drawPixel(x+i, y+j, WHITE);
}

void drawChar(byte x, byte y, char c, byte fsize)
{
  if (c == 'A' || c == 'a')
  {
    drawSquare(x, y+fsize, fsize, 4*fsize); //Left pill
    drawSquare(x+4*fsize, y+fsize, fsize, 4*fsize); //Right pill
    drawSquare(x+fsize, y+2*fsize, 3*fsize, fsize); //Bottom line
    drawSquare(x+fsize, y, 3*fsize, fsize); //Top line
    drawQuarterCircle(x+4*fsize, y, fsize, 0); //TopRight  round
    drawOuterQuarterCircle(x+4*fsize, y+fsize, fsize, 0);
    drawQuarterCircle(x, y, fsize, 3); //TopLeft  round
    drawOuterQuarterCircle(x, y, fsize, 3);
  }
  else if (c == 'B' || c == 'b')
  {
    drawSquare(x, y, fsize, 5*fsize); //Left pill
    drawSquare(x+1*fsize, y, 3*fsize, fsize); //Top line
    drawQuarterCircle(x+4*fsize, y, fsize, 0); //TopRight  round
    drawOuterQuarterCircle(x+4*fsize, y+fsize, fsize, 0);
    drawSquare(x+4*fsize, y+1*fsize, fsize, fsize); //Right top pill
    drawQuarterCircle(x+4*fsize, y+2*fsize, fsize, 1); //Middle round
    drawOuterQuarterCircle(x+4*fsize, y+2*fsize, fsize, 1);
    drawSquare(x+1*fsize, y+2*fsize, 3*fsize, fsize); //Middle line
    drawQuarterCircle(x+4*fsize, y+2*fsize, fsize, 0); //Middle round 2
    drawOuterQuarterCircle(x+4*fsize, y+3*fsize, fsize, 0);
    drawSquare(x+4*fsize, y+3*fsize, fsize, fsize); //Right bottom pill
    drawQuarterCircle(x+4*fsize, y+4*fsize, fsize, 1); //Bottom round
    drawOuterQuarterCircle(x+4*fsize, y+4*fsize, fsize, 1);
    drawSquare(x+1*fsize, y+4*fsize, 3*fsize, fsize); //Bottom line
  }
  else if (c == 'R' || c == 'r')
  {
    drawSquare(x, y, fsize, 5*fsize); //Left pill
    drawSquare(x+fsize, y, 3*fsize, fsize); //Top line
    drawQuarterCircle(x+4*fsize, y, fsize, 0); //Top round
    drawOuterQuarterCircle(x+4*fsize, y+fsize, fsize, 0);
    drawSquare(x+4*fsize, y+fsize, fsize, fsize); //Right pill
    drawQuarterCircle(x+4*fsize, y+2*fsize, fsize, 1); //Bottom round
    drawOuterQuarterCircle(x+4*fsize, y+2*fsize, fsize, 1);
    drawSquare(x+fsize, y+2*fsize, 3*fsize, fsize); //Bottom line
    drawDiagonal(x+3*fsize, y+2*fsize, 1, -3, fsize, true); //Bottom pill
  }
}

int roundUp(double a)
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

int roundDown(double a)
{
  return a;
}
