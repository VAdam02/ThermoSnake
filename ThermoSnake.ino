#include "src/DelayManager/DelayManager.h"
#include "src/Graphics/Graphics.h"
#include "src/Backstore/Backstore.h"

DelayManager delayer;
Graphics oled;
Backstore store;

void setup()
{
  Serial.begin(9600);
  delayer.begin();
  oled.begin();
}

double presstime = 0;
double data = 0;
void loop()
{
  oled.clear();
  oled.refresh();

  double x = analogRead(A1);
  double y = analogRead(A0);
  if (abs((y-512)/512) > 0.1)
  {
    double deltatime = 50;
    presstime += deltatime;
    
    if ((y-512) > 0)
    {
      data -= (presstime/1000) * deltatime / 1000;
    }
    else
    {
      data += (presstime/1000) * deltatime / 1000;
    }
  }
  else
  {
    presstime = 0;
  }
  
  oled.show();
  delayer.sleepReamingOf(50);
}

String numToString(double num, int decimals)
{
  String data = "";
  int greater = num;
  num -= greater;
  while (greater > 0)
  {
    data = String(greater % 10) + data;
    greater /= 10;
  }

  if (decimals > 0) { data += "."; }

  for (int i = 0; i < decimals; i++)
  {
    num *= 10;
    int cache = (int)num;
    data = data + String(cache);
    num -= cache;
  }
  
  return data;
}