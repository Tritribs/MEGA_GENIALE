#include <Arduino.h>
#include <LibRobus.h>
#include <MEGA.h>
#include <string.h>
#define GAUCHE 39//                    
#define MILIEU 40//
#define DROITE 38//                    


void setup() 
{
  BoardInit();          
  delay(300);           
}

void loop() 
{
 
 FOLLOW_THE_LINE();
  
  /*int EtatG = digitalRead(GAUCHE);
  int EtatM = digitalRead(MILIEU);
  int EtatD = digitalRead(DROITE);

    // Affichage simple
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
    */
}






