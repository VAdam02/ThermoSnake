#include "TempAndHum.h"
#include <Arduino.h>
#include "DHT.h"

#define DHTTYPE DHT11   // DHT 11 
#define DHTPIN 12
#define COOLDOWN 2500
#define LENGTH 12 //LENGTH * COOLDOWN / 1000 seconds

DHT dht(DHTPIN, DHTTYPE);

float DHTtemp[LENGTH];
float DHThum[LENGTH];

TempAndHum::TempAndHum() { }

void TempAndHum::begin()
{
  for (int i = 0; i < LENGTH; i++)
  {
    DHTtemp[i] = 0;
    DHThum[i] = 0;
  }
  dht.begin();

  lastTime = (unsigned int)(millis() % 65536);
}

void TempAndHum::refresh()
{
  unsigned int deltatime = (unsigned int)(millis() % 65536) - lastTime;
  if (deltatime < COOLDOWN) { return; }
  lastTime += COOLDOWN;

  for (int i = 1; i < LENGTH; i++)
  {
    DHTtemp[i-1] = DHTtemp[i];
    DHThum[i-1] = DHThum[i];
  }

  DHTtemp[LENGTH-1] = (dht.readTemperature() - 0.8);
  temperature = getTemperature(&temperatureRange);

  DHThum[LENGTH-1] = (dht.readHumidity() + 6.2);
  humidity = getHumidity(&humidityRange);
}


float TempAndHum::getCurrentTemperature()
{
  return DHTtemp[LENGTH-1];
}
float TempAndHum::getTemperature()
{
  float avg = 0;
  for (int i = 0; i < LENGTH; i++)
  {
    avg += DHTtemp[i];
  }
  avg /= LENGTH;
  return avg;
}
float TempAndHum::getTemperature(float* range)
{
  float avg = getTemperature();

  *range = 0;
  for (int i = 0; i < LENGTH; i++)
  {
    *range += abs(DHTtemp[i] - avg);
  }
  *range /= LENGTH;
  return avg;
}

float TempAndHum::getCurrentHumidity()
{
  return DHThum[LENGTH-1];
}
float TempAndHum::getHumidity()
{
  float avg = 0;
  for (int i = 0; i < LENGTH; i++)
  {
    avg += DHThum[i];
  }
  avg /= LENGTH;
  return avg;
}
float TempAndHum::getHumidity(float* range)
{
  float avg = getHumidity();

  *range = 0;
  for (int i = 0; i < LENGTH; i++)
  {
    *range += abs(DHThum[i] - avg);
  }
  *range /= LENGTH;
  return avg;
}
