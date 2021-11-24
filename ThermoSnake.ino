#include "src/DelayManager/DelayManager.h"
#include "src/Graphics/Graphics.h"

DelayManager delayer;
Graphics oled;

void setup()
{
  delayer.begin();
  oled.begin();
}

void loop()
{
  oled.clear();
  oled.refresh();

  oled.drawText(0, 0, 1, "ABCDEFGHIJKLMNOPQRST", 1);
  oled.drawText(0, 0, 9, "UVWXYZ", 1);
  oled.drawText(0, 0, 17, "0123456789", 1);
  oled.drawText(0, 0, 25, ".*%: ()", 1);
  
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
