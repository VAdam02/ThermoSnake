#include "TempAndHum.h"
#include <Arduino.h>
#include "DHT.h"

#define DHTTYPE DHT11   // DHT 11 
#define DHTPIN 17
#define LENGTH 30
#define COOLDOWN 1000

DHT dht(DHTPIN, DHTTYPE);

float temperature[LENGTH];
float humidity[LENGTH];

TempAndHum::TempAndHum() { }

void TempAndHum::begin()
{
  for (int i = 0; i < LENGTH; i++)
  {
    temperature[i] = 0;
    humidity[i] = 0;
  }
  dht.begin();
}

int lastTime = 0;
void TempAndHum::refresh(long time)
{
  //calculate deltatime
  int deltatime = (int)(time % 32768);
  if (deltatime <= lastTime)
  {
      lastTime = 0 - (32767 - lastTime);
  }
  deltatime = deltatime - lastTime;
  //calculate deltatime

  if (deltatime < COOLDOWN) { return; }
  lastTime = time % 32768;

  for (int i = 1; i < LENGTH; i++)
  {
    temperature[i-1] = temperature[i];
    humidity[i-1] = humidity[i];
  }

  //Serial.print(dht.readTemperature());
  humidity[LENGTH-1] = dht.readHumidity();
  temperature[LENGTH-1] = dht.readTemperature();
}


float TempAndHum::getCurrentTemperature()
{
  return temperature[LENGTH-1];
}

float TempAndHum::getTemperature(float* range)
{
  float avg = getTemperature();

  *range = 0;
  for (int i = 0; i < LENGTH; i++)
  {
    *range += abs(temperature[i] - avg);
  }
  *range /= LENGTH;
  return avg;
}
float TempAndHum::getTemperature()
{
  float avg = 0;
  for (int i = 0; i < LENGTH; i++)
  {
    avg += temperature[i];
  }
  avg /= LENGTH;
  return avg;
}

float TempAndHum::getCurrentHumidity()
{
  return humidity[LENGTH-1];
}


float TempAndHum::getHumidity(float* range)
{
  float avg = getHumidity();

  *range = 0;
  for (int i = 0; i < LENGTH; i++)
  {
    *range += abs(humidity[i] - avg);
  }
  *range /= LENGTH;
  return avg;
}
float TempAndHum::getHumidity()
{
  float avg = 0;
  for (int i = 0; i < LENGTH; i++)
  {
    avg += humidity[i];
  }
  avg /= LENGTH;
  return avg;
}