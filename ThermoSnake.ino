#include "src/Graphics/Graphics.h"

Graphics oled;

long time = 1;

void setup()
{
  oled.begin();
  Serial.begin(9600);
}

void loop()
{
  oled.clear();
  oled.refresh(time);

  if ((time % (4*exponentiation(2,11))) < exponentiation(2,11))
  {
    if (oled.getTargetPage() != 0)
    oled.setPage(0);
  }
  else if ((time % (4*exponentiation(2,11))) < 2*exponentiation(2,11))
  {
    if (oled.getTargetPage() != 1)
    oled.setPage(1);
  }
  else if ((time % (4*exponentiation(2,11))) < 3*exponentiation(2,11))
  {
    if (oled.getTargetPage() != 2)
    oled.setPage(2);
  }
  else
  {
    if (oled.getTargetPage() != 1)
    oled.setPage(1);
  }

  oled.drawText(0, time, 4, 2, numToString(32.5,1) + "*C", 4);

  int x;
  int fsize = 1;

  x = oled.drawText(1, time, 0, 0, "Temp: ", fsize);
  oled.drawText(1, time, x, 0, numToString(21.22, 2) + "*C", fsize);
  x = oled.drawText(1, time, 0, 0+(fsize*6), "Avgtemp: ", fsize);
  oled.drawText(1, time, x, 0+(fsize*6), numToString(22.5, 2) + "*C " + numToString(1.22, 2), fsize);

  x = oled.drawText(2, time, 0, 0+2*(fsize*6), "Hum: ", fsize);
  oled.drawText(2, time, x, 0+2*(fsize*6), numToString(45.00, 2) + "%", fsize);
  x = oled.drawText(2, time, 0, 0+3*(fsize*6), "AvgHum: ", fsize);
  oled.drawText(2, time, x, 0+3*(fsize*6), numToString(45.67, 2) + "% " + numToString(2.24, 2), fsize);
  
  /*
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
  */
  
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
