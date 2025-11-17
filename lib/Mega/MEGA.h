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
void POMPE_50ml();
void debugCapteurs();  
String LectureRFID();
   
void initLeds();
void flashLed(int pin);
int trouver_medicament(char RFID[], struct patient tableau[NOMBRE_PATIENTS]);
void initialisation_Tableau_Patient(struct patient tableau[NOMBRE_PATIENTS]);
void initBoutons();
bool isButtonPressed(int pin);

void attendPuce();
void verseEauLogique();

void initDistributeur();
void distribuerPilules();
void cycleReservoir1();
void cycleReservoir2();

//int trouver_medicament();
//void initialisation_Tableau_Patient();

/*************
DEFINITION DE LA STRUCTURE
**************/
struct patient{
    char RFID[20];
    int medicament1;
    int medicament2;
    unsigned long horaire;
    unsigned long temps_dernier_medicament;
};

#endif