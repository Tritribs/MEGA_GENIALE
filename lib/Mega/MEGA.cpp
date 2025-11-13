#include <stdio.h>
#include <LibRobus.h>
#include <math.h>
#include <string.h>
#include <MEGA.h>

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

// ---- VARIABLES RFID ----




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
capteur RFID
******************************************************************************************************************************************/


void LectureRFID(char *id_tag, char *incoming, char *i) {
    if (Serial1.available()) {
        char crecu = Serial1.read();
        switch (crecu) {
            case 0x02:
                AX_BuzzerON();
                *i = 0;
                *incoming = 1;
                break;
            case 0x03:
                AX_BuzzerOFF();
                *incoming = 0;
                for (char j = 0; j < 10; j++) Serial.print(id_tag[j]);
                Serial.println();
                break;
            default:
                if (*incoming) id_tag[(*i)++] = crecu;
                break;
        }
    }
}
/******************************************************************************************************************************************
initialisation_Tableau_Patient - Initialiser les informations sur les patients dans le tableau (base de donnees)
******************************************************************************************************************************************/

//Creation du tableau de structure avec les informations des patients
void initialisation_Tableau_Patient(struct patient tableau[NOMBRE_PATIENTS]){
    
   struct patient patient0 = {23, 1, 0, 1};//Changer le premier chiffre selon RFID
   tableau[0] = patient0;

   struct patient patient1 = {24, 0, 1, 1};
   tableau[1] = patient1;

   struct patient patient2 = {233, 1, 1, 1};
   tableau[2] = patient2;

   struct patient patient3 = {232, 1, 1, 0};
   tableau[3] = patient3;
}

/******************************************************************************************************************************************
trouver_medicament - Trouver et donner le bon medicament au bon patient
******************************************************************************************************************************************/
int trouver_medicament(struct patient tableau[NOMBRE_PATIENTS]){
    int position_tableau = -1;
    //Appeler fonction RFID
    int RFID = 233;//Remplacer par appel de la fonction

    //Verification RFID dans la base de donnes pour trouver le patient
    for (int i=0; i < NOMBRE_PATIENTS; i++){
        if (tableau[i].RFID == RFID){
            position_tableau = i;
            flashLed(PIN_VERT);
            break;
        }
    }

    //Si patient pas trouve dans la base de donnees
    if (position_tableau == -1){//ou utiliser i au lieu?
        flashLed(PIN_ROUGE);
        return 0;//Sortir de la fonction et ne pas donner de medicaments
    }
    
    //Verifier si ca fait assez de temps depuis la derniere pilule
    if (tableau[position_tableau].timeStamp == 0){//Changer valeur
        flashLed(PIN_ROUGE);
        return 0;
    }
    else{
        //Trouver les bons medicaments pour le patient
        flashLed(PIN_VERT);
        if (tableau[position_tableau].medicament1 == 1){
            //Donner le medicament1
            //Appeler fonction servo-moteurs pilule1
        }
        if (tableau[position_tableau].medicament2 == 1){
            //Donner le medicament2
            //Appeler fonction servo-moteur pilule2
        }
        
        //Changer timeStamp pour le patient qui a recu pilule
        //tableau[i].timestamp = ...;

        return 0;//ou retourner valeurs selon combinaison pilules et appeler servo-moteurs dans le main
        
    }
}
/******************************************************************************************************************************************
LED / BOUTON
******************************************************************************************************************************************/
#define PIN_ROUGE 30
#define PIN_VERT 31
#define PIN_BOUTON_ON 32 
#define PIN_BOUTON_EAU 33 

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

//initialisation des boutons (À RECHECKER)
void initBoutons(){
    pinMode(PIN_BOUTON_ON, INPUT_PULLUP); //peux juste faire pullup
    pinMode(PIN_BOUTON_EAU, INPUT_PULLUP); //peux juste faire pullup
}

//retourne vrai si le bouton (associé à la pin envoyé en argument) a été appuyé
bool isButtonPressed(int pin){
    return digitalRead(pin) == LOW;
}

/******************************************************************************************************************************************
LOGIQUE DU CODE
******************************************************************************************************************************************/
