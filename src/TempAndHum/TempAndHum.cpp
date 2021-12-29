#include "TempAndHum.h"

/*
 * EEPROM allocations:
 * SAVENAME PIN - for sensor pin's difference
 */
#define SAVENAME 'D'

TempAndHum::TempAndHum() { }

void TempAndHum::begin(byte _pin, Backstore* _store)
{
  pin = _pin;
  store = _store;

  for (int i = 0; i < LENGTH; i++)
  {
    DHTtemp[i] = NAN;
    DHThum[i] = NAN;
  }

  readConfig();

  init();
  
  lastTime = (unsigned int)(millis() % 65536);
}

void TempAndHum::refresh()
{
  unsigned int deltatime = (unsigned int)(millis() % 65536) - lastTime;
  if (deltatime < COOLDOWN) { return; }
  lastTime = (millis() % 65536);
  
  for (int i = 1; i < LENGTH; i++)
  {
    DHTtemp[i-1] = DHTtemp[i];
    DHThum[i-1] = DHThum[i];
  }
  DHTtemp[LENGTH-1] = NAN;
  DHThum[LENGTH-1] = NAN;
  read();

  DHTtemp[LENGTH-1] += TempDifference;
  temperature = getTemperature(&temperatureRange);

  DHThum[LENGTH-1] += HumDifference;
  humidity = getHumidity(&humidityRange);

  //OVERWRITE THE INCORRECT VALUES
  if (!isnan(DHTtemp[LENGTH-1]) && !isnan(DHThum[LENGTH-1]))
  {
    byte i = LENGTH - 2;
    while (i >= 0 && (isnan(DHTtemp[i]) || isnan(DHThum[i])))
    {
      DHTtemp[i] = DHTtemp[LENGTH - 1];
      DHThum[i] = DHThum[LENGTH - 1];
      i--;
    }
  }
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

void TempAndHum::readConfig()
{ 
  byte fromStore[4];
  if (store->readBytes(SAVENAME, pin, 0, 3, fromStore) != 0)
  {
    //DEBUG
    Serial.print("TempAndHum_Inicialise\n");
    //error - not found so inicialise to null mode
    store->allocateSpace(SAVENAME, pin, 4);
    fromStore[0] = 0;   //temp dif 1
    fromStore[1] = 0;   //temp dif 2
    fromStore[2] = 0;   //hum dif 1
    fromStore[3] = 0;   //hum dif 2
    store->writeBytes(SAVENAME, pin, 0, 3, fromStore);
  }
  
  TempDifference = store->reverseByteFormat(0, fromStore);
  HumDifference = store->reverseByteFormat(2, fromStore);
}




#define TIMEOUT                                                                \
  UINT32_MAX /**< Used programmatically for timeout.                           \
                   Not a timeout duration. Type: uint32_t. */

/*!
 *  @brief  Instantiates a new DHT class
 *  @param  count
 *          number of sensors
 */
/*!
 *  @brief  Setup sensor pins and set pull timings
 *  @param  usec
 *          Optionally pass pull-up time (in microseconds) before DHT reading
 *starts. Default is 55 (see function declaration in DHT.h).
 */
void TempAndHum::init() {
#ifdef __AVR
  _bit = digitalPinToBitMask(pin);
  _port = digitalPinToPort(pin);
#endif
  _maxcycles =
      microsecondsToClockCycles(1000); // 1 millisecond timeout for
                                       // reading pulses from DHT sensor.
  // Note that count is now ignored as the DHT reading algorithm adjusts itself
  // based on the speed of the processor.
  
  //BEGIN

  // set up the pins!
  pinMode(pin, INPUT_PULLUP);
}

/*!
 *  @brief  Read value from sensor or return last one from less than two
 *seconds.
 *	@return float value
 */
bool TempAndHum::read() {
  bool result = false;

  // Send start signal.  See DHT datasheet for full signal diagram:
  //   http://www.adafruit.com/datasheets/Digital%20humidity%20and%20temperature%20sensor%20AM2302.pdf

  // Go into high impedence state to let pull-up raise data line level and
  // start the reading process.
  pinMode(pin, INPUT_PULLUP);
  delay(1);

  // First set data line low for a period according to sensor type
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
  delay(20); // data sheet says at least 18ms, 20ms just to be safe

  uint32_t cycles[80];

  // End the start signal by setting data line high for 40 microseconds.
  pinMode(pin, INPUT_PULLUP);

  // Delay a moment to let sensor pull data line low.
  delayMicroseconds(55);

  // Now start reading the data line to get the value from the DHT sensor.

  // Turn off interrupts temporarily because the next sections
  // are timing critical and we don't want any interruptions.
  #if !defined(ARDUINO_ARCH_NRF52)
      noInterrupts();
  #endif

  // First expect a low signal for ~80 microseconds followed by a high signal
  // for ~80 microseconds again.
  if (expectPulse(LOW) == TIMEOUT) {
    result = false;
  }
  if (expectPulse(HIGH) == TIMEOUT) {
    result = false;
  }

  // Now read the 40 bits sent by the sensor.  Each bit is sent as a 50
  // microsecond low pulse followed by a variable length high pulse.  If the
  // high pulse is ~28 microseconds then it's a 0 and if it's ~70 microseconds
  // then it's a 1.  We measure the cycle count of the initial 50us low pulse
  // and use that to compare to the cycle count of the high pulse to determine
  // if the bit is a 0 (high state cycle count < low state cycle count), or a
  // 1 (high state cycle count > low state cycle count). Note that for speed
  // all the pulses are read into a array and then examined in a later step.
  for (int i = 0; i < 80; i += 2) {
    cycles[i] = expectPulse(LOW);
    cycles[i + 1] = expectPulse(HIGH);
  } 

  // Timing critical code is now complete.
  #if !defined(ARDUINO_ARCH_NRF52)
      interrupts();
  #endif

  // Reset 40 bits of received data to zero.
  uint8_t data[5] = { 0, 0, 0, 0, 0 };

  // Inspect pulses and determine which ones are 0 (high state cycle count < low
  // state cycle count), or 1 (high state cycle count > low state cycle count).
  for (int i = 0; i < 40; ++i) {
    uint32_t lowCycles = cycles[2 * i];
    uint32_t highCycles = cycles[2 * i + 1];
    if ((lowCycles == TIMEOUT) || (highCycles == TIMEOUT)) {
      result = false;
    }
    data[i / 8] <<= 1;
    // Now compare the low and high cycle times to see if the bit is a 0 or 1.
    if (highCycles > lowCycles) {
      // High cycles are greater than 50us low cycle count, must be a 1.
      data[i / 8] |= 1;
    }
    // Else high cycles are less than (or equal to, a weird case) the 50us low
    // cycle count so this must be a zero.  Nothing needs to be changed in the
    // stored data.
  }

  // Check sum
  if (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) { result = true; }
  else { result = false; }

  if (!result)
  {
    Serial.print("ERROR DHT11 ERROR");
    return result;
  }

  //TEMP
  DHTtemp[LENGTH-1] = data[2];
  if (data[3] & 0x80)
  {
    DHTtemp[LENGTH-1] = -1 - DHTtemp[LENGTH-1];
  }
  DHTtemp[LENGTH-1] += (data[3] & 0x0f) * 0.1;

  //HUM
  DHThum[LENGTH-1] = data[0] + data[1] * 0.1;

  return result;
}

// Expect the signal line to be at the specified level for a period of time and
// return a count of loop cycles spent at that level (this cycle count can be
// used to compare the relative time of two pulses).  If more than a millisecond
// ellapses without the level changing then the call fails with a 0 response.
// This is adapted from Arduino's pulseInLong function (which is only available
// in the very latest IDE versions):
//   https://github.com/arduino/Arduino/blob/master/hardware/arduino/avr/cores/arduino/wiring_pulse.c
uint32_t TempAndHum::expectPulse(bool level)
{
  #if (F_CPU > 16000000L)
    uint32_t count = 0;
  #else
    uint16_t count = 0; // To work fast enough on slower AVR boards
  #endif

  // On AVR platforms use direct GPIO port access as it's much faster and better
  // for catching pulses that are 10's of microseconds in length:
  #ifdef __AVR
    uint8_t portState = level ? _bit : 0;
    while ((*portInputRegister(_port) & _bit) == portState)
    {
      if (count++ >= _maxcycles)
      {
        return TIMEOUT; // Exceeded timeout, fail.
      }
    }
  // Otherwise fall back to using digitalRead (this seems to be necessary on
  // ESP8266 right now, perhaps bugs in direct port access functions?).
  #else
    while (digitalRead(pin) == level)
    {
      if (count++ >= _maxcycles)
      {
        return TIMEOUT; // Exceeded timeout, fail.
      }
    }
  #endif

  return count;
}

#if defined(TARGET_NAME) && (TARGET_NAME == ARDUINO_NANO33BLE)
  #ifndef microsecondsToClockCycles
  /*!
  * As of 7 Sep 2020 the Arduino Nano 33 BLE boards do not have
  * microsecondsToClockCycles defined.
  */
  #define microsecondsToClockCycles(a) ((a) * (SystemCoreClock / 1000000L))
  #endif
#endif