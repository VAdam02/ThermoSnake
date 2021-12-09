/*
 * EEPROM allocations:
 * TEMPSAVE 0 - for temperature sensor 0's limits
 * TEMPSAVE 1 - for temperature sensor 1's limits
 * HUMSAVE 0  - for temperature sensor 0's limits
 * HUMSAVE 1  - for temperature sensor 0's limits
 */
#define TEMPSAVE 'B'
#define HUMSAVE 'C'

#include "Watchdog.h"

Watchdog::Watchdog() { }

void Watchdog::begin(Backstore* _store, byte _tempCount, float *_TempSensors[], byte _humCount, float *_HumSensors[])
{
    wdt_enable(WDTO_8S);

    store = _store;

    tempCount = _tempCount;
    TempSensors = _TempSensors;
    TempMax = allocation(sizeof(float), tempCount);
    TempMin = allocation(sizeof(float), tempCount);
    for (int i = 0; i < tempCount; i++)
    {
        byte readed[4];
        if (store->readBytes(TEMPSAVE, i, 0, 3, readed) != 0)
        {
            store->allocateSpace(TEMPSAVE, i, 4);
            readed[0] = 127; //+128
            readed[1] = 255;
            readed[2] = 255; //-128
            readed[3] = 255;
            store->writeBytes(TEMPSAVE, i, 0, 3, readed);
        }
        TempMax[i] = store->reverseByteFormat(0, readed);
        TempMin[i] = store->reverseByteFormat(2, readed);
    }

    humCount = _humCount;
    HumSensors = _HumSensors;
    HumMax = allocation(sizeof(float), humCount);
    HumMin = allocation(sizeof(float), humCount);
    for (int i = 0; i < humCount; i++)
    {
        byte readed[4];
        if (store->readBytes(HUMSAVE, i, 0, 3, readed) != 0)
        {
            store->allocateSpace(HUMSAVE, i, 4);
            readed[0] = 127; //+128
            readed[1] = 255;
            readed[2] = 255; //-128
            readed[3] = 255;
            store->writeBytes(HUMSAVE, i, 0, 3, readed);
        }
        HumMax[i] = store->reverseByteFormat(0, readed);
        HumMin[i] = store->reverseByteFormat(2, readed);
    }
}

void Watchdog::refresh(unsigned int deltatime)
{
    wdt_reset();

    timer += deltatime;
    digitalWrite(A0, (timer > 16383 ? LOW : HIGH));

    for (byte i = 0; i < tempCount; i++)
    {
        if (*TempSensors[i] < TempMin[i] || *TempSensors[i] > TempMax[i])
        {
            Serial.print(" Alarm at Temp" + String(i));
            //TODO setalarm
            //TODO make alarm offable
        }
    }
    for (byte i = 0; i < humCount; i++)
    {
        if (*HumSensors[i] < HumMin[i] || *HumSensors[i] > HumMax[i])
        {
            Serial.print(" Alarm at Hum" + String(i));
            //TODO setalarm
            //TODO make alarm offable
        }
    }
}

void *Watchdog::allocation(byte size, byte length)
{
    void *ptr;
    do {
        Serial.print("Alloc");
        ptr = malloc(size * length);
    } while (ptr == NULL);
    return ptr;
}
