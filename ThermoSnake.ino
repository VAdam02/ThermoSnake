#include "src/Graphics/Graphics.h"

Graphics oled;

void setup()
{
  oled.begin();
  Serial.begin(9600);
}

void loop()
{
  oled.clear();
  oled.refresh(time);

  
  oled.show();
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
