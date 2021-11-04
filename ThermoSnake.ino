#include "src/DelayManager/DelayManager.h"
#include "src/TempAndHum/TempAndHum.h"

DelayManager delayer;
TempAndHum tempAndHum;

void setup()
{
  Serial.begin(9600);
  delayer.begin();
  tempAndHum.begin();
}

double offLevel = 30.0;
double onLevel = 25.0;
/*
 * 0 - Turn off
 * 1 - Turned off
 * 2 - Turn on
 * 3 - Turned on
 */
byte state = 0;
unsigned int lastTime = 0;


/*
 * STATES
 * 0 cooling
 * 1 on for x seconds and off
 * 1.5 start heating and take sample
 * 2 heating
 * 2.5 take sample and reference with the old samples
 */
#define COOLDOWN_OFF 3000
#define MAX_ON 60
#define MIN_ON 10

unsigned int on_times[] = { 0, 0 };

/*
 * 0 - Turn off
 * 1 - Turned off
 * 2 - Turn on
 * 3 - Turned on
 */
byte relayState = 1;
double valueBeforeHeating = 0;
unsigned int timeOfHeating = 0;
byte state2 = 0;
unsigned int cooldown_left = 0;
double reaction = 1.3; //modification after 10 sec under 5 minute cooldown
bool level1(double targetLevel, double difference, double curLevel, unsigned int deltatime)
{
  difference *= 0.75; //make sure to keep inside the limits
  
  //cooldown or heating in progress
  if (cooldown_left > 0 || (on_times[1] > 0 && relayState == 3))
  {
    //decrease the left variables
    if (on_times[1] > 0 && relayState == 3)
    {
      Serial.print("ONwait - ");
      return false; //nothing to do
    }
    
    if (cooldown_left < deltatime) { cooldown_left = 0; }
    else
    {
      Serial.print("COLDwait - ");
      cooldown_left -= deltatime;
      return false; //nothing to do
    }
  }


  
  //cooling
  if (state2 == 0)
  {
    Serial.print("cooling - ");
    //hot
    if (curLevel > targetLevel) { return false; } //its still hot enough so nothingto do
    //cold
    Serial.print("cold - ");
    state2++;
  }

  //on for x seconds
  if (state2 == 1 && relayState != 3)
  {
    Serial.print("heat - ");
    unsigned int onTime = (targetLevel + difference - curLevel) / reaction * 10;
    if (onTime > MAX_ON) { onTime = MAX_ON; }
    
    //it's going to be cold there
    if (curLevel < (targetLevel - difference))
    {
      Serial.print("SOS: ");
      Serial.print(onTime);
      Serial.print(" - ");
      addHeatingTask(1, onTime, 0);

      //take sample
      valueBeforeHeating = curLevel;
      timeOfHeating = getHeatingTaskTime(1);
    }
    //check (targetLevel+difference-curLevel) / (reaction) * (10sec) > MIN_ON -> power on in closed time
    else if (onTime > MIN_ON)
    {
      Serial.print("OK: ");
      Serial.print(onTime);
      Serial.print(" - ");
      //0 -- difference(0.25) ---- x*10*120 ---- 0.1C cooling in 120 seconds maximum
      addHeatingTask(1, onTime, (curLevel - (targetLevel-difference))*10*120);

      //take sample
      valueBeforeHeating = curLevel;
      timeOfHeating = getHeatingTaskTime(1);
    }
  }
  else if (state2 == 1)
  {
    state2++;
  }

  //heating in progress

  //heating finished and data analyzing
  if (state2 == 2 && on_times[1] == 0 && relayState == 1)
  {
    Serial.print("sample - ");
    //calculate avg reaction
    //TODO
    Serial.print("AVG - ");
    
    reaction = (reaction * 0.5) + ((curLevel - valueBeforeHeating) / (timeOfHeating / 10) * 0.5);
    Serial.print(reaction);
    Serial.print(" - ");
    cooldown_left = COOLDOWN_OFF;
    state2++;
  }
  
  

  if (state2 >= 3)
  {
    state2 = 0;
    return false;
  }
  
  return true;
}

#define CHANNEL
//unsigned int on_times[] = { 0, 0 };
unsigned int maxDelay_times[] = { 0, 0 };

unsigned int getHeatingTaskTime(byte ID)
{
  return on_times[ID];
}
void addHeatingTask (byte ID, unsigned int on_time, unsigned int maxDelay_time)
{
  on_times[ID] = on_time;
  if (maxDelay_times[ID] == 0 || maxDelay_times[ID] > maxDelay_time)
  {
    maxDelay_times[ID] = maxDelay_time;
  }
}

/* 
 * RETURN VALUE
 * True - new command
 * False - no new command
 */
bool level0(double offLevel, double onLevel, double curLevel, byte* state)
{
  if (curLevel < onLevel)
  {
    if (*state == 0 || *state == 1)
    {
      *state = 2;
      return true; 
    }
    return false;
    //on
  }
  else if (offLevel < curLevel)
  {
    if (*state == 2 || *state == 3)
    {
      *state = 0;
      return true; 
    }
    return false;
    //off
  }
}

double presstime = 0;
double data = 0;
void loop()
{
  tempAndHum.refresh();

  double x = analogRead(A0);
  double y = analogRead(A1);
  if ((x-512) > 200)
  {
    Serial.print("RON - ");
    relayState = 3;
  }
  if ((x-512) < -200)
  {
    Serial.print("ROFF - ");
    relayState = 1;
  }
  
  if (abs((y-512)/512) > 0.1)
  {
    double deltatime = 50;
    presstime += deltatime;
    
    if ((y-512) > 0)
    {
      data -= (presstime/1000) * deltatime / 1000;
    }
    else
    {
      data += (presstime/1000) * deltatime / 1000;
      
      if (on_times[1] < (presstime/1000) * deltatime/1000)
      {
        on_times[1] = 0;
      }
      else
      {
        on_times[1] -= (presstime/1000) * deltatime /1000;
      }
      
    }
    Serial.print(data);
    Serial.print("\n");
  }
  else
  {
    presstime = 0;
  }
  
  
  unsigned int deltatime = (unsigned int)(millis() % 65536) - lastTime;
  if (deltatime > 1000)
  {
    float range = 0;
    /*
    //Serial.print(tempAndHum.getTemperature(&range));
    Serial.print(data);
    Serial.print(" - ");
    Serial.print(level0(offLevel, onLevel, data, &state));
    Serial.print(" - ");
    Serial.print(state);
    Serial.print("\n");
    */

    maxDelay_times[1] = 0;
    
    range = 0;
    //Serial.print(tempAndHum.getTemperature(&range));
    Serial.print(data);
    Serial.print(" - ");
    Serial.print(level1(30.0, 10.0, data, 100));
    Serial.print(" - ");
    Serial.print(state2);
    Serial.print(" - ");
    Serial.print(relayState);
    Serial.print(" - ");
    Serial.print(maxDelay_times[1]);
    Serial.print("\n");

    
    /*
    Serial.print("\n");
    float range = 0;
    float val = tempAndHum.getTemperature(&range);
    Serial.print(tempAndHum.getCurrentTemperature());
    Serial.print(" "); 
    Serial.print(val);
    Serial.print(" "); 
    Serial.print(range);
    Serial.print(" ");
    val = tempAndHum.getHumidity(&range);
    Serial.print(tempAndHum.getCurrentHumidity());
    Serial.print(" "); 
    Serial.print(val);
    Serial.print(" "); 
    Serial.print(range);
    */
    lastTime += 1000;
  }

  //TODO make better debug
  if (state == 2)
  {
    digitalWrite(2, HIGH);
    state = 3;
  }
  else if (state == 0)
  {
    digitalWrite(2, LOW);
    state = 1;
  }
  
  delayer.sleepReamingOf(50);
}
