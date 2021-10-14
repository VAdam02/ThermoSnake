#include "src/DelayManager/DelayManager.h"
#include "src/Graphics/Graphics.h"
#include "src/TempAndHum/TempAndHum.h"

DelayManager delayer;
Graphics oled;
TempAndHum tempAndHum;

long time = 1;

void setup()
{
  delayer.begin();
  oled.begin();
  tempAndHum.begin();
}

void loop()
{
  oled.clear();
  oled.refresh(time);
  tempAndHum.refresh(time);

  if ((time % (4*exponentiation(2,12))) < 2*exponentiation(2,12))
  {
    if (oled.getTargetPage() != 0)
    oled.setPage(0);
  }
  else if ((time % (4*exponentiation(2,12))) < 3*exponentiation(2,12))
  {
    if (oled.getTargetPage() != 1)
    oled.setPage(1);
  }
  else
  {
    if (oled.getTargetPage() != 2)
    oled.setPage(2);
  }
  
  oled.drawText(0, time, 4, 7, numToString(tempAndHum.getTemperature(),1) + "*C", 4);
  
  int x;
  int fsize = 1;

  float range;
  float val = tempAndHum.getTemperature(&range);
  oled.drawText(1, time, 0, 0, "Temp: " + numToString(tempAndHum.getCurrentTemperature(), 2) + "*C", fsize);
  oled.drawText(1, time, 0, 0+(fsize*6), "Avg: " + numToString(val, 2) + "*C", fsize);
  oled.drawText(1, time, 0, 0+2*(fsize*6), "Dif: " + numToString(range, 2), fsize);

  val = tempAndHum.getHumidity(&range);
  oled.drawText(2, time, 0, 0, "Hum: " + numToString(tempAndHum.getCurrentHumidity(), 2) + "%", fsize);
  oled.drawText(2, time, 0, 0+1*(fsize*6), "Avg: " + numToString(val, 2) + "%", fsize);
  oled.drawText(2, time, 0, 0+2*(fsize*6), "Dif: " + numToString(range, 2), fsize);
  
  oled.show();
  delayer.sleepReamingOf(50);
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
