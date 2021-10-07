#include "src/Graphics/Graphics.h"
#include "src/TempAndHum/TempAndHum.h"

Graphics oled;
TempAndHum tempAndHum;

long time = 1;

void setup()
{
  oled.begin();
  tempAndHum.begin();
}

void loop()
{
  oled.clear();
  oled.refresh(time);
  tempAndHum.refresh(time);

  if ((time % exponentiation(2,14)) < exponentiation(2,13))
  {
    oled.drawText(time, 4, 2, numToString(tempAndHum.getTemperature(),1) + "*C", 4);
  }
  else
  {
    int x;
    int fsize = 1;

    float range;
    float val = tempAndHum.getTemperature(&range);
    x = oled.drawText(time, 0, 0, "Temp: ", fsize);
    oled.drawText(time, x, 0, numToString(tempAndHum.getCurrentTemperature(), 2) + "*C", fsize);
    x = oled.drawText(time, 0, 0+(fsize*6), "AvgTemp: ", fsize);
    oled.drawText(time, x, 0+(fsize*6), numToString(val, 2) + "*C " + numToString(range, 2), fsize);

    val = tempAndHum.getHumidity(&range);
    x = oled.drawText(time, 0, 0+2*(fsize*6), "Hum: ", fsize);
    oled.drawText(time, x, 0+2*(fsize*6), numToString(tempAndHum.getCurrentHumidity(), 2) + "%", fsize);
    x = oled.drawText(time, 0, 0+3*(fsize*6), "AvgHum: ", fsize);
    oled.drawText(time, x, 0+3*(fsize*6), numToString(val, 2) + "% " + numToString(range, 2), fsize);
  }
  
  oled.show();
  delay(50);
  time += 50;
}

long exponentiation(long base, long exponent)
{
  long value = 1;
  for (int i = 0; i < exponent; i++)
  {
    value *= base;
  }
  return value;
}

String numToString(double num, int decimals)
{
  String data = "";
  int greater = num;
  num -= greater;
  if (greater < 1) { data = "0"; }
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
