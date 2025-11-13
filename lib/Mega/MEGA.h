#ifndef LIBROBOT_H
#define LIBROBOT_H

#include <Arduino.h>
#include <librobus.h>
#include <MEGA.h>
#include <string.h>

/**********
CONSTANTES 
**********/
#define MOTEUR_GAUCHE 0
#define MOTEUR_DROITE 1
#define PIN_ROUGE 30
#define PIN_VERT 31
#define PIN_BOUTON_ON 32 
#define PIN_BOUTON_EAU 33 
#define PIN_CAPTEUR_IR 0

//Definir nombre maximum de patients
#define NOMBRE_PATIENTS 4


/*************
FONCTION
**************/
void FOLLOW_THE_LINE();
void POMP_50ml();
void debugCapteurs();  
void LectureRFID(char *id_tag, char *incoming, char *i);
   
void initLeds();
void flashLed(int pin);
void initBoutons();
bool isButtonPressed(int pin);

void timerPourBoutonVerreDEau();
void attendPuce();
void actionnePompeSiVerrePresent();
void timerApresEau();

float litDonneesCapteurIREnCm();
float retourneDistanceCmMoyenne();
bool vraiSiVerreEstLa();

int trouver_medicament();
void initialisation_Tableau_Patient();

/*************
DEFINITION DE LA STRUCTURE
**************/
struct patient{
    int RFID;
    int medicament1;
    int medicament2;
    unsigned int timeStamp;
};

#endif