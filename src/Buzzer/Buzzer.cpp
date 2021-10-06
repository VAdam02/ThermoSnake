#include "Buzzer.h"
#include <Arduino.h>

Buzzer::Buzzer() {}

int _pin = 2;
bool taskActive = false;
bool taskSound = false;
int task[] = {100, 1, 1, 1, 1};

void Buzzer::begin(byte pin)
{
    Serial.begin(9600);
    _pin = pin;

    tone(_pin, 500);
    delay(50);
    noTone(_pin);
}

int lastTime = 0;
void Buzzer::refresh(long time)
{
    //calculate deltatime
    int deltatime = (int)(time % 32768);
    if (deltatime <= lastTime)
    {
        lastTime = 0 - (32767 - lastTime);
    }
    deltatime = deltatime - lastTime;
    lastTime = time % 32768;
    //calculate deltatime

    if (taskActive)
    {
        task[4] += deltatime;

        if (task[3] < task[4])
        {
            noTone(_pin);
            taskSound = false;
            taskActive = false;
        }
        else
        {
            int cycleTime = task[4] % (task[1] + task[2]);
            if (taskSound != (cycleTime < task[1]))
            {
                if (cycleTime < task[1])
                {
                    tone(_pin, task[0]);
                    taskSound = true;
                }
                else
                {
                    noTone(_pin);
                    taskSound = false;
                }
            }
        }
    }
}

void Buzzer::alarm(int hz, int sound, int silent, int time)
{
    taskActive = false;
    task[0] = hz;
    task[1] = sound;
    task[2] = silent;
    task[3] = time;
    task[4] = 0;
    taskActive = true;
}

void Buzzer::mute()
{
    alarm(100, 0, 1000, 1);
}

void Buzzer::sayError()
{
    alarm(100, 100, 100, 400);
}