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

//Definir nombre maximum de patients
#define NOMBRE_PATIENTS 4


/*************
FONCTION
**************/
void FOLLOW_THE_LINE();
void POMP_50ml();
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