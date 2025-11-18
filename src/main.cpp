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
  

  /*   
  Serial.println("Test du ID-12 sur UART2 (RX2 / Digital 17)");         
  delay(300);           
    */         
}

void loop() 
{
  //departCodeMegaGenial();
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
    FOLLOW_THE_LINE();
  }
  
}

void testsTristan(){
  
  int EtatG = digitalRead(GAUCHE);
  int EtatM = digitalRead(MILIEU);
  int EtatD = digitalRead(DROITE);

  //Affichage simple
  Serial.print("G: "); Serial.print(EtatG);
  Serial.print(" | M: "); Serial.print(EtatM);
  Serial.print(" | D: "); Serial.println(EtatD);

  // Affichage plus détaillé
   if (EtatG == 0 && EtatM == 0 && EtatD == 0) {
  Serial.println("TOUS les capteurs détectent la ligne !");
  } 
  else if (EtatG == 0 && EtatM == 0) {
  Serial.println("Virage GAUCHE détecté");
  } 
  else if (EtatD == 0 && EtatM == 0) {
  Serial.println("Virage DROITE détecté");
  } 
  else if (EtatM == 0) {
  Serial.println("Robot centré sur la ligne");
  }
  delay(250);
}





