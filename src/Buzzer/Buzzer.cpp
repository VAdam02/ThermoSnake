#include "Buzzer.h"
#include <Arduino.h>

Buzzer::Buzzer() {}

byte _pin = 2;
bool taskActive = false;
bool taskSound = false;
unsigned int task[] = {100, 1, 1, 1};

void Buzzer::begin(byte pin)
{
    _pin = pin;

    tone(_pin, 500);
    delay(50);
    noTone(_pin);

    Buzzer_lastTime = (unsigned int)(millis() % 65536);
}

unsigned int Buzzer_lastTime = 0;
void Buzzer::refresh()
{
    unsigned int deltatime = (unsigned int)(millis() % 65536) - Buzzer_lastTime;

    if (taskActive)
    {
        if (task[3] < deltatime)
        {
            noTone(_pin);
            taskSound = false;
            taskActive = false;
        }
        else
        {
            if (taskSound != ((deltatime % (task[1] + task[2])) < task[1]))
            {
                if ((deltatime % (task[1] + task[2])) < task[1])
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

void Buzzer::alarm(unsigned int hz, unsigned int sound, unsigned int silent, unsigned int time)
{
    task[0] = hz;
    task[1] = sound;
    task[2] = silent;
    task[3] = time;
    Buzzer_lastTime = (unsigned int)(millis() % 65536);
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