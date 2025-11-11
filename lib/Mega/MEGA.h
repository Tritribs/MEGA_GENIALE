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

/*************
FONCTION
**************/
void FOLLOW_THE_LINE();
void POMP_50ml();
void debugCapteurs();  
   
void initLeds();
void flashLed(int pin);
void initBoutons();
bool isButtonPressed(int pin);


#endif