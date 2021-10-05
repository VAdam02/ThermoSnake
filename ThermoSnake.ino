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

int time = 0;

void setup()
{
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.clearDisplay();

  /*
  int a = 0;
  char c = 'I';
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
  */

  int fsize = 1;
  drawText(time, 0, 1, "abcdefghijklm", fsize);
  drawText(time, 0, 1+1*6*fsize, "nopqrstuvwxyz", fsize);
  drawText(time, 0, 1+2*6*fsize, "0123456789*.:", fsize);
  drawText(time, 0, 1+3*6*fsize, "% ", fsize);
  display.display();
  delay(3000);
}

void loop()
{
  display.clearDisplay();
  
  /*
  int fsize = 1;
  drawText(time, 0, 1, "abcdefghijklm", fsize);
  drawText(time, 0, 1+1*6*fsize, "nopqrstuvwxyz", fsize);
  drawText(time, 0, 1+2*6*fsize, "0123456789.*%", fsize);
  drawText(time, 0, 1+3*6*fsize, ": ", fsize);
  */

  
  int a = 0;
  char c = '4';
  drawChar(time, a, 1, c, 6);
  a = 6 * 6 + a;
  drawChar(time, a, 1, c, 5);
  a = 6 * 5 + a;
  drawChar(time, a, 1, c, 4);
  a = 6 * 4 + a;
  drawChar(time, a, 1, c, 3);
  a = 6 * 3 + a;
  drawChar(time, a, 1, c, 2);
  a = 6 * 2 + a;
  drawChar(time, a, 1, c, 1);
  

  /*
  for (int x=0; x < 128; x++)
  {
    display.drawPixel(x, 0, SSD1306_WHITE);
    display.drawPixel(x, 19, SSD1306_WHITE);
  }

  for (int y=0; y < 32; y++)
  {
    display.drawPixel(0, y, SSD1306_WHITE);
    display.drawPixel(19, y, SSD1306_WHITE);
  }
  
  int fsize = 3;
  int x = 2;
  int y = 2;
  drawDiagonal(x, y, 5, -5, fsize, true); //Bottom pill
  drawDiagonal(x, y+4*fsize-1, 5, 5, fsize, true); //Bottom pill
  */
  
  display.display();

  delay(100);
  time++;
}

void drawDiagonal(byte absoluteX1, byte absoluteY1, byte relativeX2, float relativeY2, byte width, bool alignRight)
{
  for (int j = 0; abs(j) < abs(relativeY2*width)-width; j+=(relativeY2/abs(relativeY2))) //-y
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
    else if (direction % 4 == 2) { display.drawPixel(x +(width-i), y +(j-1), WHITE); }
    else { display.drawPixel(x +(width-i), y +(width-j), WHITE); }
  }
}
void drawOuterQuarterCircle(byte x, byte y, byte width, byte direction)
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

void drawSquare(byte x, byte y, byte xwidth, byte yheight)
{
  for (int i = 0; i < xwidth; i++)
  for (int j = 0; j < yheight; j++)
  display.drawPixel(x+i, y+j, WHITE);
}

void drawText(int time, byte x, byte y, String text, byte fsize)
{
  for (int i = 0; i < text.length(); i++)
  {
    drawChar(time, x + (i*fsize*6), y, text[i], fsize);
  }
}
void drawChar(int time, byte x, byte y, char c, byte fsize)
{
  time = time % (60*4);

  byte side = time / 60;
  //global side step (time-side * 60)
  //local step size
  byte step = round((double)(time-side * 60) * fsize /60);
  //byte step = time % fsize;

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
    drawQuarterCircle(x+4*fsize, y+4*fsize, fsize, 1); //Bottom round
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
  else
  {
    drawSquare(x, y, 5*fsize, 5*fsize);
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
