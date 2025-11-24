#include <Arduino.h>
#include <LibRobus.h>
#include <MEGA.h>
#include <string.h>
#define GAUCHE 39//                    
#define MILIEU 40//
#define DROITE 38//                    

char id_tag[20];
char incoming;
char i;
bool bumperArr;
bool megaGenialStarted = false;

void setup() 
{
  Serial.begin(9600);
  Serial1.begin(9600);
  BoardInit();  
  Serial.println("Test du ID-12 sur UART2 (RX2 / Digital 17)"); 
  delay(300);
  pinMode(41, OUTPUT);
  initBoutons();     
  initLeds();   
  initialisation_Tableau_Patient(tableauPatients);

  attendPuce();

  initDistributeur();
  /*   
  Serial.println("Test du ID-12 sur UART2 (RX2 / Digital 17)");         
  delay(300);           
    */         
}

void loop() 
{
  //POMPE_50ml();
  //delay(2000);
  //FOLLOW_THE_LINE();
  //testsTristan();
  //departCodeMegaGenial();
  /*cycleReservoir1();
  delay(1000);
 
  cycleReservoir2();
  delay(1000);
*/

  //testsTristan();

  /*  
  char* tag = LectureRFID();
  if (tag != NULL) {
    Serial.print("Tag lu : ");
    Serial.println(tag);
  }
  */     
}



void departCodeMegaGenial(){
  bumperArr = ROBUS_IsBumper(3);

  
  if (bumperArr)
  {
    megaGenialStarted=true;
  }

  if(megaGenialStarted== true)
  {
    FOLLOW_THE_LINE();
  }
}

