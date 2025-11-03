#include <Arduino.h>
#include <LibRobus.h>
#include <MEGA.h>
#include <string.h>


void setup() 
{
  BoardInit();          
  delay(300);           
}

void loop() 
{
  FOLLOW_THE_LINE();
}






