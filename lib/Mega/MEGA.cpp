#include <stdio.h>
#include <LibRobus.h>
#include <math.h>
#include <string.h>

/************
PID
************/

#define MOTEUR_GAUCHE 0
#define MOTEUR_DROITE 1

// ---- CONSTANTES ----
const float distanceParPulse = 0.000147262 ; // roue 3 pouces, encodeur 3200 pulses
const float Kp = 1.2;                     // gain PID
const unsigned long interval = 50;        // ms
const float vitesseAvance = 0.3;
const float vitesseRotation = 0.25;

// ---- VARIABLES PID ----
float Vt0 = vitesseAvance;
float Vt1 = vitesseAvance;
static unsigned long lastTime = 0;



float vitesse = 0.4; // vitesse par défaut

// conversion cm -> ticks encodeur
int cmToTicks(float cm) {
  return cm * 13581; // ajuster selon roue
}

// conversion angle -> ticks encodeur
int angleToTicks(float angle) {
  float distance = (angle / 360.0) * 0.195 * PI; // circonférence roue = 0.195 m
  return distance * 13581;
}

// ====================================================================
// AVANCER D’UNE DISTANCE DONNÉE (cm)
// ====================================================================
void avance(float distanceCm) {
    ENCODER_Reset(MOTEUR_GAUCHE);
    ENCODER_Reset(MOTEUR_DROITE);

    // ← CHANGEMENT ICI
    int32_t lastPulse0 = ENCODER_Read(MOTEUR_GAUCHE);
    int32_t lastPulse1 = ENCODER_Read(MOTEUR_DROITE);

    lastTime = millis();
    float distanceM = distanceCm / 100.0 * 2.067;

    Vt0 = vitesseAvance;
    Vt1 = vitesseAvance;

    while (true) {
        unsigned long now = millis();
        if (now - lastTime >= interval) {
            float deltaT = (now - lastTime) / 1000.0;
            lastTime = now;

            int32_t pulse0 = ENCODER_Read(MOTEUR_GAUCHE);
            int32_t pulse1 = ENCODER_Read(MOTEUR_DROITE);

            float Vp0 = (pulse0 - lastPulse0) * distanceParPulse / deltaT;
            float Vp1 = (pulse1 - lastPulse1) * distanceParPulse / deltaT;

            lastPulse0 = pulse0;
            lastPulse1 = pulse1;

            float Erreur0 = Vt0 - Vp0;
            float Erreur1 = Vt1 - Vp1;

            float commande0 = constrain(Vt0 + Kp * Erreur0, -1.0, 1.0);
            float commande1 = constrain(Vt1 + Kp * Erreur1, -1.0, 1.0);

            MOTOR_SetSpeed(MOTEUR_GAUCHE, commande0);
            MOTOR_SetSpeed(MOTEUR_DROITE, commande1);

            float distG = pulse0 * distanceParPulse;
            float distD = pulse1 * distanceParPulse;
            if ((distG + distD) / 2.0 >= distanceM) break;
        }
    }

    //MOTOR_SetSpeed(MOTEUR_GAUCHE, 0);
    //MOTOR_SetSpeed(MOTEUR_DROITE, 0);
}

// ====================================================================
// TOURNER D’UN ANGLE DONNÉ (°)
// gauche = true → gauche, false → droite
// ====================================================================
void tourne(int angleDeg, bool tourneGauche) {
    ENCODER_Reset(MOTEUR_GAUCHE);
    ENCODER_Reset(MOTEUR_DROITE);

    int32_t lastPulse0 = ENCODER_Read(MOTEUR_GAUCHE);
    int32_t lastPulse1 = ENCODER_Read(MOTEUR_DROITE);

    lastTime = millis();
    const float pulsesParDegre = 22.1;  // à calibrer selon ton robot
    float cible = angleDeg * pulsesParDegre;

    // Consignes de vitesse
    float consigne = vitesseRotation;

    while (true) {
        unsigned long now = millis();
        if (now - lastTime >= interval) {
            float deltaT = (now - lastTime) / 1000.0;
            lastTime = now;

            int32_t pulse0 = ENCODER_Read(MOTEUR_GAUCHE);
            int32_t pulse1 = ENCODER_Read(MOTEUR_DROITE);

            float Vp0 = (pulse0 - lastPulse0) * distanceParPulse / deltaT;
            float Vp1 = (pulse1 - lastPulse1) * distanceParPulse / deltaT;

            lastPulse0 = pulse0;
            lastPulse1 = pulse1;

            // ---- Définir les consignes selon le sens de rotation ----
            if (tourneGauche) {
                Vt0 = -consigne;
                Vt1 = consigne;
            } else {
                Vt0 = consigne;
                Vt1 = -consigne;
            }

            // ---- PID proportionnel ----
            float Erreur0 = Vt0 - Vp0;
            float Erreur1 = Vt1 - Vp1;

            float commande0 = constrain(Vt0 + Kp * Erreur0, -1.0, 1.0);
            float commande1 = constrain(Vt1 + Kp * Erreur1, -1.0, 1.0);
            
            MOTOR_SetSpeed(MOTEUR_GAUCHE, commande0);
            MOTOR_SetSpeed(MOTEUR_DROITE, commande1);
        }

        // ---- Condition de fin : vérifier si l'angle visé est atteint ----
        int32_t absG = abs(ENCODER_Read(MOTEUR_GAUCHE));
        int32_t absD = abs(ENCODER_Read(MOTEUR_DROITE));
        if ((absG + absD) / 2.0 >= cible) break;
    }

    MOTOR_SetSpeed(MOTEUR_GAUCHE, 0);
    MOTOR_SetSpeed(MOTEUR_DROITE, 0);
}

/******************************************************************************************************************************************
SUIVEUR DE LIGNE ET CONNEXE
******************************************************************************************************************************************/
  
void FOLLOW_THE_LINE(void)
{

#define MOTEUR_GAUCHE 0
#define MOTEUR_DROITE 1

// Pins capteurs
#define CAP_GAUCHE 39//                    38
#define CAP_MILIEU 40// rouler a l'envers, || mettre les vitesses negative et inerser les virages
#define CAP_DROITE 38//                    39

// Vitesses
float vitesseBase = 0.3;
float correction = 0.15; 


    int EtatG = digitalRead(CAP_GAUCHE);
    int EtatM = digitalRead(CAP_MILIEU);
    int EtatD = digitalRead(CAP_DROITE);

    
    Serial.print("G: "); Serial.print(EtatG);
    Serial.print(" | M: "); Serial.print(EtatM);
    Serial.print(" | D: "); Serial.println(EtatD);

    // ---- états ----
    if (EtatG == 0) { // Capteur gauche détecte ligne
        MOTOR_SetSpeed(MOTEUR_GAUCHE, vitesseBase - correction);
        MOTOR_SetSpeed(MOTEUR_DROITE, vitesseBase + correction);
    }
    else if (EtatM == 0) { // Capteur milieu détecte ligne
        MOTOR_SetSpeed(MOTEUR_GAUCHE, vitesseBase);
        MOTOR_SetSpeed(MOTEUR_DROITE, vitesseBase);
    }
    else if (EtatD == 0) { // Capteur droite détecte ligne
        MOTOR_SetSpeed(MOTEUR_GAUCHE, vitesseBase + correction);
        MOTOR_SetSpeed(MOTEUR_DROITE, vitesseBase - correction);
    }
    else { // Aucun capteur détecte la ligne
      
        MOTOR_SetSpeed(MOTEUR_GAUCHE, 0.2);
        MOTOR_SetSpeed(MOTEUR_DROITE, 0.2);
    }


    ///-----------------------------------
    if(EtatD==0 && EtatM==0 && EtatG==0)
    {
        MOTOR_SetSpeed(1,0);
        MOTOR_SetSpeed(0,0);
        delay(2500);
        tourne(180, true);
        delay(2500);
        tourne(90, false);
    }


    // ---- virages ----
    if (EtatG == 0 && EtatM == 0) {
        // Virage 90° gauche
        avance(6);
        tourne(90, true);
        return; 
    }
    else if (EtatD == 0 && EtatM == 0) {
        // Virage 90° droite
        avance(6);
        tourne(90, false);
        return;
    }
}




/******************************************************************************************************************************************
POMPE EAU
******************************************************************************************************************************************/

/*
la pompe doit rester submerger!!!!!!!!!!!
floatation device? pour limter la distribution d'eau
flow caculator?
*/
#define POMPE 41

void pump_50ml()

{
    pinMode(POMPE, OUTPUT);

}

/******************************************************************************************************************************************
LED / BOUTON
******************************************************************************************************************************************/
#define PIN_ROUGE 30
#define PIN_VERT 31

//initialisation de la led rouge et verte
void initLeds(){
    pinMode(PIN_ROUGE, OUTPUT);
    pinMode(PIN_VERT, OUTPUT);
}

//fait flasher la led qui correspond à la pin envoyé en argument
void flashLed(int pin){
    for(int i = 0; i < 5; i++){
        digitalWrite(pin, HIGH);
        delay(100);
        digitalWrite(pin, LOW);
        delay(100);
    }
}