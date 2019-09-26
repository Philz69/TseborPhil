#include <Arduino.h>
#include <LibRobUS.h>

float lastMotorSpeed;
const double pi = 3.1415926535897932384626433832795028841; //Remplacer par PI qui est dans la libraire Arduino.h
const double DIAMETRE_ROUE = 0.077; //Le diametre d'une roue en metres
const double DISTANCE_ENTRE_ROUES = 0.189; //La distance entre les roues paralles en metres
const double NB_PULSE_TOUR = 3200; //Le nombre de pulse de l'encodeur pour 1 tour de roue
double correctionMoteurGauche = 1.048;
double correctionMoteurDroit = 1;

void avancerDistance(double distanceVoulue, double vitesseVoulue);
void TournerAngle1Moteur(double angleVoulu, double vitesseVoulue);


//Tourne 1 des 2 moteurs du robot. 0 = LEFT, 1 = RIGHT


void setup()
{
  BoardInit();
  Serial.begin(9600);
  delay(1500);
}

void loop() 
{
  if (ROBUS_IsBumper(3))
  {
    avancerDistance(5, 0.30);
  }

   if (ROBUS_IsBumper(1))
  {
    Serial.println("Bumper 1");
      TournerAngle1Moteur(-90, 0.05);
  }
  
  if (ROBUS_IsBumper(0))
  {
    Serial.println("Bumper 0");
    TournerAngle1Moteur(-45, 0.05);
  }

 
}

void avancerDistance(double distanceVoulue, double vitesseVoulue)
{
  int delaiMesure = 50; //Le delai de mesure de la distance en millisecondes
  double diffDistance;  //La difference de pulse entre l'encodeur et 350
  double distanceParcourue = 0; //La distance parcourue en metres

  double valeurEncodeur0;
  double valeurEncodeur1;

  int nbPulseVoulu = (vitesseVoulue * (delaiMesure/1000)) / ((1/NB_PULSE_TOUR) * pi*DIAMETRE_ROUE);
  MOTOR_SetSpeed(LEFT, 0.25 * correctionMoteurGauche);
  MOTOR_SetSpeed(RIGHT, 0.25 * correctionMoteurDroit);
  double derniereVitesseMoteur0 = 0.25;
  double derniereVitesseMoteur1 = 0.25;
  
  while( distanceParcourue < (distanceVoulue - 0.05) && !ROBUS_IsBumper(2)) //S'execute tant que la distance voulue n'est pas atteinte et que le bumper avant n'est pas touche
  {
    ENCODER_ReadReset(LEFT);
    ENCODER_ReadReset(RIGHT);
    delay(delaiMesure);
    valeurEncodeur0 = ENCODER_Read(LEFT);
    valeurEncodeur1 = ENCODER_Read(RIGHT);

    if (valeurEncodeur0 != nbPulseVoulu) 
    {
      diffDistance = 350 - valeurEncodeur0;
      MOTOR_SetSpeed(LEFT, derniereVitesseMoteur0 + (diffDistance * 0.0005));
      derniereVitesseMoteur0 = derniereVitesseMoteur0 + (diffDistance * 0.0005);
    }
    if (valeurEncodeur1 != nbPulseVoulu) 
    {
      diffDistance = 350 - valeurEncodeur1;
      MOTOR_SetSpeed(RIGHT, derniereVitesseMoteur1 + (diffDistance * 0.0005));
      derniereVitesseMoteur1 = derniereVitesseMoteur1 + (diffDistance * 0.0005);
    }
    distanceParcourue = distanceParcourue + (((valeurEncodeur0 + valeurEncodeur1)/2)/NB_PULSE_TOUR) * pi*DIAMETRE_ROUE;
  }

  MOTOR_SetSpeed(LEFT, 0); //Le robot s'arrete une fois que la distance est atteinte ou quand son bumper avant est frappé
  MOTOR_SetSpeed(RIGHT, 0);

  delay(500);
}

void TournerAngle1Moteur(double angleVoulu, double vitesseVoulue)
{
  double delaiMesure = 25; //Le delai de mesure de la distance en millisecondes
  double diffDistance;  //La difference entre nbPulseVoulu et valeurEncodeur
  double distanceParcourue = 0; //La distance parcourue en metres
  double derniereVitesseMoteur0;
  double derniereVitesseMoteur1;
  double moyenneEncodeur = 0;
  double valeurEncodeur0;
  double valeurEncodeur1;
  double nbPulseVoulu0;
  double nbPulseVoulu1;

  double distanceVoulue = fabs((angleVoulu / 360) * PI * DISTANCE_ENTRE_ROUES);
  double nbPulseVoulu = (vitesseVoulue * (delaiMesure/1000)) / ((1/NB_PULSE_TOUR) * pi*DIAMETRE_ROUE); 
 

  if (angleVoulu > 0) {
    MOTOR_SetSpeed(LEFT, 0.1 * correctionMoteurGauche);
    MOTOR_SetSpeed(RIGHT, -0.1 * correctionMoteurDroit);
    derniereVitesseMoteur0 = 0.1;
    derniereVitesseMoteur1 = -0.1;
    nbPulseVoulu0 = nbPulseVoulu;
    nbPulseVoulu1 = nbPulseVoulu * -1;
  }

  if (angleVoulu < 0) {
    MOTOR_SetSpeed(LEFT, -0.1 * correctionMoteurGauche);
    MOTOR_SetSpeed(RIGHT, 0.1 * correctionMoteurDroit);
    derniereVitesseMoteur0 = -0.1;
    derniereVitesseMoteur1 = 0.1;
    nbPulseVoulu0 = nbPulseVoulu * -1;
    nbPulseVoulu1 = nbPulseVoulu;
  }

  //S'execute tant que la distance voulue n'est pas atteinte et que le bumper avant n'est pas touche
  while( (fabs(distanceParcourue) < fabs(distanceVoulue)) && !ROBUS_IsBumper(2))
  {
    ENCODER_ReadReset(LEFT);
    ENCODER_ReadReset(RIGHT);
    delay(delaiMesure);
    valeurEncodeur0 = ENCODER_Read(LEFT);
    valeurEncodeur1 = ENCODER_Read(RIGHT);

    if (valeurEncodeur0 != nbPulseVoulu) 
    {
      diffDistance = nbPulseVoulu0 - valeurEncodeur0;
      MOTOR_SetSpeed(LEFT, derniereVitesseMoteur0 + (diffDistance * 0.0005));
      derniereVitesseMoteur0 = derniereVitesseMoteur0 + (diffDistance * 0.0005);
    }

    if (valeurEncodeur1 != nbPulseVoulu) 
    {
      diffDistance = nbPulseVoulu1 - valeurEncodeur1;
      MOTOR_SetSpeed(RIGHT, derniereVitesseMoteur1 + (diffDistance * 0.0005));
      derniereVitesseMoteur1 = derniereVitesseMoteur1 + (diffDistance * 0.0005);
    }

    moyenneEncodeur = (fabs(valeurEncodeur0) + fabs(valeurEncodeur1))/2;
    distanceParcourue = distanceParcourue + (moyenneEncodeur/NB_PULSE_TOUR) * pi*DIAMETRE_ROUE;
    Serial.println(distanceParcourue, 10); 

  }
  
  MOTOR_SetSpeed(LEFT, 0); //Le robot s'arrete une fois que la distance est atteinte ou quand son bumper avant est frappé
  MOTOR_SetSpeed(RIGHT, 0);

  delay(500);
}
