#include "src/DelayManager/DelayManager.h"
#include "src/Backstore/Backstore.h"

DelayManager delayer;
Backstore store;

void setup()
{
  Serial.begin(9600);
  
  Serial.print("\n");
  Serial.print("Start");
  store.inicialise(256);
  
  
  Serial.print("\n");
  Serial.print(store.allocateSpace('B', 2, 160));
  Serial.print("\n");
  
  Serial.print("\n");
  Serial.print(store.allocateSpace('B', 3, 150));
  Serial.print("\n");

  Serial.print("\n");
  Serial.print(store.allocateSpace('B', 4, 140));
  Serial.print("\n");
  
  Serial.print("\n");
  Serial.print(store.allocateSpace('B', 5, 130));
  Serial.print("\n");

  Serial.print("\n");
  Serial.print(store.allocateSpace('B', 6, 160));
  Serial.print("\n");

  Serial.print("\n");
  Serial.print(store.freeUpSpace('B', 5));
  Serial.print("\n");
  
  Serial.print("\n");
  Serial.print(store.freeUpSpace('B', 2));
  Serial.print("\n");

  Serial.print("\n");
  Serial.print(store.freeUpSpace('B', 3));
  Serial.print("\n");

  Serial.print("\n");
  Serial.print(store.freeUpSpace('B', 6));
  Serial.print("\n");


  store.defragmentStorage();
  
  /*
  Serial.print("\n");
  store.allocateSpace('B', 2, 10);
  Serial.print("\n");
  store.mem();
  */
  

  /*
  Serial.print("\n");
  byte data[] = {analogRead(A0), analogRead(A1), analogRead(A2), analogRead(A3), analogRead(A4), analogRead(A5), analogRead(A6), 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
  Serial.print(store.writeBytes('B', 2, 0, 20, data));
  for (int i = 0; i < 16; i++)
  {
    data[i] = 43;
  }
  Serial.print("\n");
  store.mem();
  
  
  Serial.print("\n");
  byte asd = 20;
  byte data2[asd];
  Serial.print(store.readBytes('B', 2, 5, asd-1, data2));
  Serial.print("\n");
  for (int i = 0; i < asd; i++)
  {
    Serial.print(data2[i]);
    Serial.print(" ");
  }
  Serial.print("\n");
  */

  delayer.begin();
}

void loop()
{
  delayer.sleepReamingOf(50);
}
