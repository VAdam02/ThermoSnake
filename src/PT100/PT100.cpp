#include "PT100.h"
#include <Arduino.h>

#define EMPTY -255
#define COOLDOWN 100
#define LENGTH 30 //LENGTH * COOLDOWN / 1000 seconds

#define PIN0 A2
#define PIN1 A3

#define RESISTANCEATREF1 116
#define CELSIUSATREF1 24
#define RESISTANCEATREF0 120
#define CELSIUSATREF0 34

float PT100temp[LENGTH];

PT100::PT100() { }

void PT100::begin()
{
  for (int i = 0; i < LENGTH; i++)
  {
    PT100temp[i] = EMPTY;
  }

  lastTime = (unsigned int)(millis() % 65536);
}

void PT100::refresh()
{
  unsigned int deltatime = (unsigned int)(millis() % 65536) - lastTime;
  if (deltatime < COOLDOWN) { return; }
  lastTime += COOLDOWN;

  for (int i = 1; i < LENGTH; i++)
  {
    PT100temp[i-1] = PT100temp[i];
  }

  PT100temp[LENGTH-1] = convertValueToCelsius(analogRead(PIN0), analogRead(PIN1));
  if (PT100temp[0] == EMPTY)
  {
    int i = 0;
    while (PT100temp[i] == EMPTY)
    {
      PT100temp[i] = PT100temp[LENGTH-1];
      i++;
    }
  }
  temperature = getTemperature(&temperatureRange);
}


float PT100::getCurrentTemperature()
{
  return PT100temp[LENGTH-1];
}
float PT100::getTemperature()
{
  float avg = 0;
  for (int i = 0; i < LENGTH; i++)
  {
    avg += PT100temp[i];
  }
  avg /= LENGTH;
  return avg;
}
float PT100::getTemperature(float* range)
{
  float avg = getTemperature();

  *range = 0;
  for (int i = 0; i < LENGTH; i++)
  {
    *range += abs(PT100temp[i] - avg);
  }
  *range /= LENGTH;
  return avg;
}

float PT100::convertValueToCelsius(unsigned int val1, unsigned int val2)
{
    float voltage = (float)(val1+val2)*5/1024/2;
    float resistance = (voltage*150)/(5-voltage);
    //DEBUG
    Serial.print(" - ");
    Serial.print(resistance, DEC);
    Serial.print(" - ");
    return (((resistance - RESISTANCEATREF0) / (RESISTANCEATREF1 - RESISTANCEATREF0)) * (CELSIUSATREF1-CELSIUSATREF0)) + CELSIUSATREF0;
    return 0;
}
