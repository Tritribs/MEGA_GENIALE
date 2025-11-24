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
const float vitesseAvance = 0.2;
const float vitesseRotation = 0.12;

// ---- VARIABLES PID ----
float Vt0 = vitesseAvance;
float Vt1 = vitesseAvance;
static unsigned long lastTime = 0;

// ---- VARIABLES BASE DE DONNÉES PATIENTS ----
struct patient tableauPatients[NOMBRE_PATIENTS];
char idPharmacien[] = "1600CA59E86D"; //À CHANGER

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
  

#define MOTEUR_GAUCHE 0
#define MOTEUR_DROITE 1

// Pins capteurs
#define GAUCHE 39//                    
#define MILIEU 40// 
#define DROITE 38//                    

// Vitesses
float vitesseBase = 0.20;
float correction = 0.1; 

void FOLLOW_THE_LINE(void)
{


    int EtatG = digitalRead(GAUCHE);
    int EtatM = digitalRead(MILIEU);
    int EtatD = digitalRead(DROITE);

    
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


    ///-------------Fonction Megagenial----------
    if(EtatD==0 && EtatM==0 && EtatG==0)
    {
        avance(6);
        MOTOR_SetSpeed(1,0);
        MOTOR_SetSpeed(0,0);
        delay(2500);
        tourne(180, true);
        attendPuce(); //démarre séquence puce
        tourne(90, false);

    }


    // ---- virages ----
    if (EtatG == 0 && EtatM == 0) {
        // Virage 90° gauche
        avance(1.8);
        delay(500);
        tourne(90, true);
        delay(500);
        return; 
    }
    else if (EtatD == 0 && EtatM == 0) {
        // Virage 90° droite
        avance(1.8);
        delay(500);
        tourne(90, false);
        delay(500);
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

void POMPE_50ml()


{
    digitalWrite(POMPE, HIGH);
    delay(2000);
    digitalWrite(POMPE, LOW);
}

void pompeON(){
    digitalWrite(POMPE, HIGH);
}

void pompeOFF(){
    digitalWrite(POMPE, LOW);
}

/******************************************************************************************************************************************
capteur RFID
******************************************************************************************************************************************/

char* LectureRFID() {
    static char id_tag[20];
    static char i = 0;
    static bool incoming = false;

    if (Serial1.available()) {
        char crecu = Serial1.read();

        switch (crecu) {
        case 0x02: // Début de trame
            AX_BuzzerON();
            i = 0;
            incoming = true;
            break;

        case 0x03: // Fin de trame
            AX_BuzzerOFF();
            incoming = false;
            id_tag[i-2] = '\0'; //faut mettre i - 2 sinon il y a des caractères random à la fin 
            Serial.println(id_tag);
              // terminer la chaîne C
            return id_tag;      // ***retourner le char[] directement***

        default:
            if (incoming && i < sizeof(id_tag) - 1) {
                id_tag[i++] = crecu;
            }
            break;
        }
    }

    return NULL; // rien reçu
}


/******************************************************************************************************************************************
initialisation_Tableau_Patient - Initialiser les informations sur les patients dans le tableau (base de donnees)
******************************************************************************************************************************************/
 
//Creation du tableau de structure avec les informations des patients
void initialisation_Tableau_Patient(struct patient tableau[NOMBRE_PATIENTS]){
   
   struct patient patient0 = {"00008926A708", 1, 0, 1, 999999999};
   tableau[0] = patient0; 
 
   struct patient patient1 = {"48007593EF41", 0, 1, 1, 999999999};
   tableau[1] = patient1;
 
   struct patient patient2 = {"000088E89BFB", 1, 1, 1, 999999999};
   tableau[2] = patient2;
 
   struct patient patient3 = { "", 1, 1, 2, 999999999};
    strcpy(patient3.RFID, idPharmacien);
   tableau[3] = patient3;
}
 
/******************************************************************************************************************************************
trouver_medicament - Trouver et donner le bon medicament au bon patient
******************************************************************************************************************************************/
int trouver_medicament(char RFID[], struct patient tableau[NOMBRE_PATIENTS]){
    int position_tableau = -1;
 
    //Verification RFID dans la base de donnes pour trouver le patient
    for (int i=0; i < NOMBRE_PATIENTS; i++){
        
        /*
        Serial.print("i : ");
        Serial.println(i);
        Serial.print("Rfid bd : *");
        Serial.print(tableau[i].RFID);
        Serial.println("*");
        Serial.print("rfid argument : *");
        Serial.print(RFID);
        Serial.println("*");
        Serial.println("-------------------------------------");
        */

        if (strcmp(RFID, tableau[i].RFID) == 0){
            position_tableau = i;
            
            //Serial.println("a trouvé la puce");
            //flashLed(PIN_VERT);
            break;
        }
    }
 
    //Si patient pas trouve dans la base de donnees
    if (position_tableau == -1){
        flashLed(PIN_ROUGE);
        return 0;//Sortir de la fonction et ne pas donner de medicaments ou appeler servo-moteurs avec 0,0
    }
   
    //Verifier si ca fait assez de temps depuis le dernier medicament
    unsigned long temps_maintenant = millis();
    if ((temps_maintenant - (tableau[position_tableau].temps_dernier_medicament) >= (tableau[position_tableau].horaire * 60 * 1000)) || (tableau[position_tableau].temps_dernier_medicament == 999999999)){
        flashLed(PIN_VERT);
 
        //Changer temps_dernier_medicament pour le patient qui a recu son medicament
        tableau[position_tableau].temps_dernier_medicament = temps_maintenant;
        
        Serial.print("nbr méd #1 : ");
        Serial.println(tableau[position_tableau].medicament1);
        Serial.print("nbr méd #2 : ");
        Serial.println(tableau[position_tableau].medicament2);

        //appeler fonction pour distribuer les bons medicaments pour le patient
        distribuerPilules(tableau[position_tableau].medicament1, tableau[position_tableau].medicament2);
        verseEauLogique();
        return 0;
    }
    else{
        Serial.println("trop tôt");
        flashLed(PIN_ROUGE);
        return 0;//ou appeler servo-moteurs avec 0,0
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
Distributeur de pilules
******************************************************************************************************************************************/
#define SERVO_ID 0

#define ANGLE_R1   45
#define ANGLE_R2   135
#define ANGLE_DROP 95

#define DELAI_PICK 1000
#define DELAI_DROP 2000

void initDistributeur(){
    BoardInit();
    SERVO_Enable(SERVO_ID);
    SERVO_SetAngle(SERVO_ID, ANGLE_DROP);
    delay(DELAI_DROP);
    }

    void cycleReservoir1(){
    SERVO_SetAngle(SERVO_ID, ANGLE_R1);
    delay(DELAI_PICK);  
    SERVO_SetAngle(SERVO_ID, ANGLE_DROP);
    delay(DELAI_DROP);
    }

    void cycleReservoir2(){
    SERVO_SetAngle(SERVO_ID, ANGLE_R2);
    delay(DELAI_PICK);
    SERVO_SetAngle(SERVO_ID, ANGLE_DROP);
    delay(DELAI_DROP);
    }

    void cycleReservoir12(){
    SERVO_SetAngle(SERVO_ID, ANGLE_R1);
    delay(DELAI_PICK);  
    SERVO_SetAngle(SERVO_ID, ANGLE_DROP);
    delay(DELAI_DROP);
    SERVO_SetAngle(SERVO_ID, ANGLE_R2);
    delay(DELAI_PICK);
    SERVO_SetAngle(SERVO_ID, ANGLE_DROP);
    delay(DELAI_DROP);
    }

    void distribuerPilules(int nbrMed1, int nbrMed2){
 
    for (int i = 0; i < nbrMed1; i++){
        cycleReservoir1();
    }
 
    for (int i = 0; i < nbrMed2; i++){
        cycleReservoir2();
    }
    }

/******************************************************************************************************************************************
LOGIQUE DU CODE
******************************************************************************************************************************************/
/*
Cette fonction attend qu'une puce soit scannée. Lorsque ça arrive, la séquence de base de donnée est démarrée.
Cette fonction doit être appelée lorsque le suiveur de ligne est arrivé à la chambre d'un patient.
*/
void attendPuce(){
    char* puce = LectureRFID();
    int chrono = 0;
    int tempsAttente = 60000; 

    //loop pour attendre pendant 1 minute (si rien se passe, part)

    int ancienTemps = millis();
    while (puce == NULL && chrono < tempsAttente){
        int tempsActuel = millis();
    
        chrono += (tempsActuel - ancienTemps);
        ancienTemps = tempsActuel;
        puce = LectureRFID();
    }
    if (strcmp(puce, idPharmacien) == 0)
    {
        attendRecharge();
    } else {
        trouver_medicament(puce, tableauPatients);
    }

}

/*
Cette fonction gère le versage d'eau, et le timer pour cette action
*/
void verseEauLogique(){
    int chronoEau = 0;
    int tempsMaxEau = 3000; //à changer pour ce que tristan aura trouvé
    int tempsDepart = millis();
    int tempsAttente = 60000; //attend et si pas d'actions après 30 secondes, retourne suiveur de ligne

    //loop pour attendre 30 secondes
    while (chronoEau < tempsMaxEau && (millis() - tempsDepart) < tempsAttente){
        int ancienTemps = millis();

        Serial.println(isButtonPressed(PIN_BOUTON_EAU));
        Serial.println(chronoEau < tempsMaxEau);
        Serial.println(ROBUS_IsBumper(3));

        //loop pour verser eau et calculer temps
        while(isButtonPressed(PIN_BOUTON_EAU) && (chronoEau < tempsMaxEau) && ROBUS_IsBumper(3)){
            int tempsActuel = millis();
        
            tempsDepart = 0;
            chronoEau += (tempsActuel - ancienTemps);

            /*
            Serial.print("chrono eau : ");
            Serial.println(chronoEau);
            Serial.print("ancien temps : ");
            Serial.println(ancienTemps);
            Serial.print("temps actuel : ");
            Serial.println(tempsActuel);
            Serial.println("**********************************************");
            */

            ancienTemps = tempsActuel;

            Serial.println("pompe à ON");
            pompeON();
        }
        Serial.println("pompe à OFF");
        pompeOFF();

        if(isButtonPressed(PIN_BOUTON_EAU) && !ROBUS_IsBumper(3)){
            flashLed(PIN_ROUGE);
        }
    }
    delay(3000);

}

/*
Cette fonction est appelée lorsque la puce scannée est celle du pharmacien.
Cette fonction attend que la recharge des médicaments soit complétée (donc que le bumper
arrière soit pesé pour 3 secondes)
*/
void attendRecharge(){
    int chrono = 0;
    int tempsAppuyer = 3000; 
    bool bumperArr;

    //loop pour attendre que pèse 3 secondes
    while (chrono < tempsAppuyer){
        int ancienTemps = millis();
        bumperArr = ROBUS_IsBumper(3);

        //loop pour calculer le nombre de temps que le bumper est appuyé
        while((chrono < tempsAppuyer) && bumperArr){
            int tempsActuel = millis();
        
            chrono += (tempsActuel - ancienTemps);
            ancienTemps = tempsActuel;
            bumperArr = ROBUS_IsBumper(3);
        }

        if (chrono < tempsAppuyer)
        {
            chrono = 0;
        }
        
    }
    flashLed(PIN_VERT);
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
