void setup()
{
  
}

int Delay_lastTime = 0;
void loop()
{
  //Code goes here
  
  //calculate deltatime
  int deltatime = (int)(millis() % 32768);
  if (deltatime <= Delay_lastTime)
  {
      Delay_lastTime = 0 - (32767 - Delay_lastTime);
  }
  deltatime = deltatime - Delay_lastTime;
  //calculate deltatime

  if (deltatime < 50)
  {
    delay(50-deltatime);
  }
  else
  {
    //FIXME probably something go really wrong with time consume so manage this
  }
  Delay_lastTime += 50;
}
