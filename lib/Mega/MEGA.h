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

//Definir nombre maximum de patients
#define NOMBRE_PATIENTS 4


/*************
FONCTION
**************/
void FOLLOW_THE_LINE();
void POMP_50ml();
void debugCapteurs();  
String LectureRFID();
   
void initLeds();
void flashLed(int pin);
int trouver_medicament(struct patient tableau[NOMBRE_PATIENTS]);
void initialisation_Tableau_Patient(struct patient tableau[NOMBRE_PATIENTS]);
void initBoutons();
bool isButtonPressed(int pin);

void initDistributeur();
void cycleReservoir1();
void cycleReservoir2();
void cycleReservoir12();

int trouver_medicament();
void initialisation_Tableau_Patient();

/*************
DEFINITION DE LA STRUCTURE
**************/
struct patient{
    int RFID;//changer pour char
    int medicament1;
    int medicament2;
    unsigned int timeStamp;
};

#endif