#include <Arduino.h>
#include <LibRobUS.h>

const double pi = 3.1415926535897932384626433832795028841; //Remplacer par PI qui est dans la libraire Arduino.h
const double DIAMETRE_ROUE = 0.077; //Le diametre d'une roue en metres
const double DISTANCE_ENTRE_ROUES = 0.189; //La distance entre les roues paralles en metres
const double NB_PULSE_TOUR = 3200; //Le nombre de pulse de l'encodeur pour 1 tour de roue
const double VITESSE_DEPART_TOURNER = 0.1; //La vitesse de depart des moteurs pour tourner
const double VITESSE_DEPART_AVANCER = 0.1; //La vitesse de depart des moteurs pour tourner

double correctionMoteurGauche = 1.048;
double correctionMoteurDroit = 1;

const double VITESSE_AVANCER = 0.25;
const double VITESSE_TOURNER = 0.25;

void avancerDistance(double distanceVoulue, double vitesseVoulue);
void tournerAngle(double angleVoulu, double vitesseVoulue);
void ajusterMoteurs(int coteMoteur, double nbPulseVoulu, double nbCycles, double valeurEncodeur, double *nbPulseTotal, double *derniereVitesseMoteur);


//Tourne 1 des 2 moteurs du robot. 0 = LEFT, 1 = RIGHT


void setup()
{
    BoardInit();
    Serial.begin(9600);
    delay(1500);

void loop() 
{
    if (ROBUS_IsBumper(3) && !ROBUS_IsBumper(2))
    {
        avancerDistance(1.17, VITESSE_AVANCER);
        tournerAngle(-90, VITESSE_TOURNER);
        avancerDistance(0.85, VITESSE_AVANCER);
        tournerAngle(90, VITESSE_TOURNER);
        avancerDistance(0.90, VITESSE_AVANCER);
        tournerAngle(45, VITESSE_TOURNER);
        avancerDistance(1.77, VITESSE_AVANCER);
        tournerAngle(-90, VITESSE_TOURNER);
        avancerDistance(0.50, VITESSE_AVANCER);
        tournerAngle(45, VITESSE_TOURNER);
        avancerDistance(1.30, VITESSE_AVANCER);

        tournerAngle(180, VITESSE_TOURNER);

        avancerDistance(1.30, VITESSE_AVANCER);
        tournerAngle(-45, VITESSE_TOURNER);
        avancerDistance(0.55, VITESSE_AVANCER);
        tournerAngle(90, VITESSE_TOURNER);
        avancerDistance(1.77, VITESSE_AVANCER);
        tournerAngle(-45, VITESSE_TOURNER);
        avancerDistance(0.90, VITESSE_AVANCER);
        tournerAngle(-90, VITESSE_TOURNER);
        avancerDistance(0.85, VITESSE_AVANCER);
        tournerAngle(90, VITESSE_TOURNER);
        avancerDistance(1.17, VITESSE_AVANCER);
    }

    if (ROBUS_IsBumper(1))
    {
        Serial.println("Bumper 1");
        tournerAngle(-90, 0.25);
    }

    if (ROBUS_IsBumper(0))
    {
        Serial.println("Bumper 0");
        tournerAngle(-45, 0.25);
    }
}

void avancerDistance(double distanceVoulue, double vitesseVoulue)
{
    double i = 1;
    double delaiMesure = 50; //Le delai de mesure de la distance en millisecondes
    double diffDistance;  //La difference de pulse entre l'encodeur et 350
    double distanceParcourue = 0; //La distance parcourue en metres

    double valeurEncodeurGauche;
    double valeurEncodeurDroit;

    double nbPulseTotalVoulu;
    double nbPulseTotalGauche = 0;
    double nbPulseTotalDroit = 0;

    double nbPulseVoulu = (vitesseVoulue * (delaiMesure/1000)) / ((1/NB_PULSE_TOUR) * pi*DIAMETRE_ROUE);

    MOTOR_SetSpeed(LEFT, VITESSE_DEPART_AVANCER * correctionMoteurGauche);
    MOTOR_SetSpeed(RIGHT, VITESSE_DEPART_AVANCER * correctionMoteurDroit);

    double derniereVitesseMoteurGauche = VITESSE_DEPART_AVANCER * correctionMoteurGauche;
    double derniereVitesseMoteurDroit = VITESSE_DEPART_AVANCER * correctionMoteurDroit;

    while( distanceParcourue < (distanceVoulue) && !ROBUS_IsBumper(2)) //S'execute tant que la distance voulue n'est pas atteinte et que le bumper avant n'est pas touche
    {
        ENCODER_ReadReset(LEFT);
        ENCODER_ReadReset(RIGHT);

        delay(delaiMesure);

        valeurEncodeurGauche = ENCODER_Read(LEFT);
        valeurEncodeurDroit = ENCODER_Read(RIGHT);

        nbPulseTotalVoulu = i * nbPulseVoulu;

        if (valeurEncodeurGauche != nbPulseVoulu) 
        {
            ajusterMoteurs(LEFT, nbPulseVoulu, i, valeurEncodeurGauche, &nbPulseTotalGauche, &derniereVitesseMoteurGauche);
        }

        if (valeurEncodeurDroit != nbPulseVoulu) 
        {
            ajusterMoteurs(RIGHT, nbPulseVoulu, i, valeurEncodeurDroit, &nbPulseTotalDroit, &derniereVitesseMoteurDroit);
        }

        distanceParcourue = distanceParcourue + (((valeurEncodeurGauche + valeurEncodeurDroit)/2)/NB_PULSE_TOUR) * pi*DIAMETRE_ROUE;
        i++;
    }

    MOTOR_SetSpeed(LEFT, 0); //Le robot s'arrete une fois que la distance est atteinte ou quand son bumper avant est frappé
    MOTOR_SetSpeed(RIGHT, 0);

    delay(50);
}

void tournerAngle(double angleVoulu, double vitesseVoulue)
{
    int i = 1;
    double delaiMesure = 50; //Le delai de mesure de la distance en millisecondes
    double diffDistance;  //La difference entre nbPulseVoulu et valeurEncodeur
    double distanceParcourue = 0; //La distance parcourue en metres

    double derniereVitesseMoteurGauche;
    double derniereVitesseMoteurDroit;

    double moyenneEncodeur = 0;

    double valeurEncodeurGauche;
    double valeurEncodeurDroit;

    double nbPulseVouluGauche = 0;
    double nbPulseVouluDroit = 0;

    double nbPulseTotalGauche = 0;
    double nbPulseTotalDroit =0;


    double distanceVoulue = (fabs((angleVoulu / 360) * PI * DISTANCE_ENTRE_ROUES))/2;
    double nbPulseVoulu = (vitesseVoulue * (delaiMesure/1000)) / ((1/NB_PULSE_TOUR) * pi*DIAMETRE_ROUE); 


    if (angleVoulu > 0) {
        MOTOR_SetSpeed(LEFT, VITESSE_DEPART_TOURNER * correctionMoteurGauche);
        MOTOR_SetSpeed(RIGHT, VITESSE_DEPART_TOURNER * -1 * correctionMoteurDroit);

        derniereVitesseMoteurGauche = VITESSE_DEPART_TOURNER;
        derniereVitesseMoteurDroit = VITESSE_DEPART_TOURNER * -1;

        nbPulseVouluGauche = nbPulseVoulu;
        nbPulseVouluDroit = nbPulseVoulu * -1;
    }


    if (angleVoulu < 0) {
        MOTOR_SetSpeed(LEFT, VITESSE_DEPART_TOURNER * -1 * correctionMoteurGauche);
        MOTOR_SetSpeed(RIGHT, VITESSE_DEPART_TOURNER * correctionMoteurDroit);

        derniereVitesseMoteurGauche = VITESSE_DEPART_TOURNER * -1;
        derniereVitesseMoteurDroit = VITESSE_DEPART_TOURNER;

        nbPulseVouluGauche = nbPulseVoulu * -1;
        nbPulseVouluDroit = nbPulseVoulu;
    }

    //S'execute tant que la distance voulue n'est pas atteinte et que le bumper avant n'est pas touche
    while( (fabs(distanceParcourue) < fabs(distanceVoulue)) && !ROBUS_IsBumper(2))
    {
        ENCODER_ReadReset(LEFT);
        ENCODER_ReadReset(RIGHT);

        delay(delaiMesure);

        valeurEncodeurGauche = ENCODER_Read(LEFT);
        valeurEncodeurDroit = ENCODER_Read(RIGHT);

        if (valeurEncodeurGauche != nbPulseVouluGauche) 
        {
            ajusterMoteurs(LEFT, nbPulseVouluGauche, i, valeurEncodeurGauche, &nbPulseTotalGauche, &derniereVitesseMoteurGauche);
        }

        if (valeurEncodeurDroit != nbPulseVouluDroit) 
        {
            ajusterMoteurs(RIGHT, nbPulseVouluDroit, i, valeurEncodeurDroit, &nbPulseTotalDroit, &derniereVitesseMoteurDroit);
        }

        moyenneEncodeur = (fabs(valeurEncodeurGauche) + fabs(valeurEncodeurDroit))/2;

        distanceParcourue = distanceParcourue + (moyenneEncodeur/NB_PULSE_TOUR) * pi*DIAMETRE_ROUE;

        i++;
    }

    MOTOR_SetSpeed(LEFT, 0); //Le robot s'arrete une fois que la distance est atteinte ou quand son bumper avant est frappé
    MOTOR_SetSpeed(RIGHT, 0);

    delay(50);
}

void ajusterMoteurs(int coteMoteur, double nbPulseVoulu, double nbCycles, double valeurEncodeur, double *nbPulseTotal, double *derniereVitesseMoteur)
{ 
    double diffDistance = nbPulseVoulu - valeurEncodeur;
    (*nbPulseTotal) = (*nbPulseTotal) + valeurEncodeur;
    double correctionInteg = nbPulseVoulu * nbCycles - (*nbPulseTotal) ;
    MOTOR_SetSpeed(coteMoteur, (*derniereVitesseMoteur) + (diffDistance * 0.0001) + (correctionInteg * 0.00005));
    (*derniereVitesseMoteur) = (*derniereVitesseMoteur) + (diffDistance * 0.0001) + (correctionInteg * 0.00005);
}
