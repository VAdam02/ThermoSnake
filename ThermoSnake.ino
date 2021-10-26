#include "src/DelayManager/DelayManager.h"
#include "src/Graphics/Graphics.h"
#include "src/TempAndHum/TempAndHum.h"
#include "src/Backstore/Backstore.h"

DelayManager delayer;
Graphics oled;
TempAndHum tempAndHum;
Backstore store;

char charss[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y'};

void setup()
{
  delayer.begin();
  oled.begin();
  tempAndHum.begin();
  Serial.begin(9600);

  //store.inicialise(64);
  byte address[1];
  
  if (store.readBytes('A', 0, 0, 0, address) != 0)
  {
    store.inicialise(64);
    store.mem();
    Serial.print("\n");
    Serial.print(store.allocateSpace('A', 0, 4));
    byte asd[] = {10};
    Serial.print("\n");
    Serial.print(store.writeBytes('A', 0, 0, 0, asd));
    Serial.print("\n");
  }
  store.mem();
}


unsigned int lastTime = 0;
unsigned long lastSave = 0;
void loop()
{
  oled.clear();
  oled.refresh();
  tempAndHum.refresh();

  
  unsigned long delta = millis() - lastSave;
  if (delta >= (900000))
  {
    //write
    byte address[1];
    store.readBytes('A', 0, 0, 0, address);
    int add = address[0];
    add++;
    char c = charss[add/10];
    byte num = add % 10;
    store.allocateSpace(c, num, 4);

    byte content[] = {0, 0, 0, 0};
    float range;
    float val = tempAndHum.getTemperature(&range);
    content[0] = (byte)(val);
    content[1] = (byte)((val-((byte)(val)))*100);

    val = tempAndHum.getHumidity(&range);
    content[2] = (byte)(val);
    content[3] = (byte)((val-((byte)(val)))*100);

    store.writeBytes(c, num, 0, 3, content);

    address[0] = address[0] + 1;
    
    store.writeBytes('A', 0, 0, 0, address);
    
    
    lastSave += (900000);
  }






  unsigned int deltatime = (unsigned int)(millis() % exponentiation(2,15)) - lastTime;

  if (deltatime < 4*exponentiation(2,12))
  {
    if (oled.getTargetPage() != 0)
    oled.setPage(0);
  }
  else if (deltatime < 6*exponentiation(2,12))
  {
    if (oled.getTargetPage() != 1)
    oled.setPage(1);
  }
  else if (deltatime < 8*exponentiation(2,12))
  {
    if (oled.getTargetPage() != 2)
    oled.setPage(2);
  }
  else
  {
    lastTime += exponentiation(2,14);
  }
  
  oled.drawText(0, 4, 7, numToString(tempAndHum.getTemperature(),1) + "*C", 4);
  
  int x;
  int fsize = 1;

  float range;
  float val = tempAndHum.getTemperature(&range);
  oled.drawText(1, 0, 0, "Temp: " + numToString(tempAndHum.getCurrentTemperature(), 2) + "*C", fsize);
  oled.drawText(1, 0, 0+(fsize*6), "Avg: " + numToString(val, 2) + "*C", fsize);
  oled.drawText(1, 0, 0+2*(fsize*6), "Dif: " + numToString(range, 2), fsize);
  oled.drawText(1, 0, 0+3*(fsize*6), String(delta), fsize);

  val = tempAndHum.getHumidity(&range);
  oled.drawText(2, 0, 0, "Hum: " + numToString(tempAndHum.getCurrentHumidity(), 2) + "%", fsize);
  oled.drawText(2, 0, 0+1*(fsize*6), "Avg: " + numToString(val, 2) + "%", fsize);
  oled.drawText(2, 0, 0+2*(fsize*6), "Dif: " + numToString(range, 2), fsize);
  oled.drawText(2, 0, 0+3*(fsize*6), String(delta), fsize);
  
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
