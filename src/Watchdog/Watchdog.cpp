#include "Watchdog.h"

Watchdog::Watchdog() { }

void Watchdog::begin()
{
    wdt_enable(WDTO_8S);
}

void Watchdog::refresh(unsigned int deltatime)
{
    wdt_reset();

    timer += deltatime;
    digitalWrite(A0, (timer % 32768 > 16384 ? LOW : HIGH));
}