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
void ajustementMoteur(int coteMoteur, double nbPulseVoulu, double nbCycles, double valeurEncodeur, double *nbPulseTotal, double *derniereVitesseMoteur);


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
    avancerDistance(1, 0.5);
  }

   if (ROBUS_IsBumper(1))
  {
    Serial.println("Bumper 1");
      TournerAngle1Moteur(-90, 0.25);
  }
  
  if (ROBUS_IsBumper(0))
  {
    Serial.println("Bumper 0");
    TournerAngle1Moteur(-45, 0.25);
  }

 
}

void avancerDistance(double distanceVoulue, double vitesseVoulue)
{
  double i = 1;
  double delaiMesure = 50; //Le delai de mesure de la distance en millisecondes
  double diffDistance;  //La difference de pulse entre l'encodeur et 350
  double distanceParcourue = 0; //La distance parcourue en metres

  double valeurEncodeur0;
  double valeurEncodeur1;

  double nbPulseTotalVoulu;
  double nbPulseTotal0 = 0;
  double nbPulseTotal1 = 0;

  double correctionInteg0 = 0;
  double correctionInteg1 = 0;

  double nbPulseVoulu = (vitesseVoulue * (delaiMesure/1000)) / ((1/NB_PULSE_TOUR) * pi*DIAMETRE_ROUE);
  Serial.println(nbPulseVoulu);
  MOTOR_SetSpeed(LEFT, 0.1 * correctionMoteurGauche);
  MOTOR_SetSpeed(RIGHT, 0.1 * correctionMoteurDroit);
  double derniereVitesseMoteur0 = 0.1* correctionMoteurGauche;
  double derniereVitesseMoteur1 = 0.1 * correctionMoteurDroit;
  while( distanceParcourue < (distanceVoulue) && !ROBUS_IsBumper(2)) //S'execute tant que la distance voulue n'est pas atteinte et que le bumper avant n'est pas touche
  {
    ENCODER_ReadReset(LEFT);
    ENCODER_ReadReset(RIGHT);
    delay(delaiMesure);
    valeurEncodeur0 = ENCODER_Read(LEFT);
    valeurEncodeur1 = ENCODER_Read(RIGHT);
    nbPulseTotalVoulu = i * nbPulseVoulu;

    if (valeurEncodeur0 != nbPulseVoulu) 
    {
      ajustementMoteur(LEFT, nbPulseVoulu, i, valeurEncodeur0, &nbPulseTotal0, &derniereVitesseMoteur0);
    }

    if (valeurEncodeur1 != nbPulseVoulu) 
    {
      ajustementMoteur(RIGHT, nbPulseVoulu, i, valeurEncodeur1, &nbPulseTotal1, &derniereVitesseMoteur1);
    }


  

    Serial.println(nbPulseTotal0);
    Serial.println(nbPulseTotalVoulu);
    Serial.println(correctionInteg0);


    distanceParcourue = distanceParcourue + (((valeurEncodeur0 + valeurEncodeur1)/2)/NB_PULSE_TOUR) * pi*DIAMETRE_ROUE;
    i++;
  }

  MOTOR_SetSpeed(LEFT, 0); //Le robot s'arrete une fois que la distance est atteinte ou quand son bumper avant est frappé
  MOTOR_SetSpeed(RIGHT, 0);

  delay(500);
}

void TournerAngle1Moteur(double angleVoulu, double vitesseVoulue)
{
  int i = 1;
  double delaiMesure = 50; //Le delai de mesure de la distance en millisecondes
  double diffDistance;  //La difference entre nbPulseVoulu et valeurEncodeur
  double distanceParcourue = 0; //La distance parcourue en metres
  double derniereVitesseMoteur0;
  double derniereVitesseMoteur1;
  double moyenneEncodeur = 0;
  double valeurEncodeur0;
  double valeurEncodeur1;
  double nbPulseVoulu0 = 0;
  double nbPulseVoulu1 = 0;
  double nbPulseTotal0 = 0;
  double nbPulseTotal1 =0;
  

  double distanceVoulue = (fabs((angleVoulu / 360) * PI * DISTANCE_ENTRE_ROUES))/2;
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
  Serial.println("nbPulseVoulu0");
  Serial.println(nbPulseVoulu0);
  Serial.println("nbPulseVoulu1");
  Serial.println(nbPulseVoulu1);
  //S'execute tant que la distance voulue n'est pas atteinte et que le bumper avant n'est pas touche
  while( (fabs(distanceParcourue) < fabs(distanceVoulue)) && !ROBUS_IsBumper(2))
  {
    ENCODER_ReadReset(LEFT);
    ENCODER_ReadReset(RIGHT);
    delay(delaiMesure);
    valeurEncodeur0 = ENCODER_Read(LEFT);
    valeurEncodeur1 = ENCODER_Read(RIGHT);

    if (valeurEncodeur0 != nbPulseVoulu0) 
    {
      /*Serial.print("\nvaleurEncodeur0: ");
      Serial.print(valeurEncodeur0);
      Serial.print(" | nbPulseVoulu: ");
      Serial.print(nbPulseVoulu);
      Serial.print(" | avant  derniereVitesseMoteur0: ");
      Serial.print(derniereVitesseMoteur0);*/
      
      ajustementMoteur(LEFT, nbPulseVoulu0, i, valeurEncodeur0, &nbPulseTotal0, &derniereVitesseMoteur0);
      Serial.print(" | apres  derniereVitesseMoteur0: ");
      Serial.print(derniereVitesseMoteur0);
    }

    if (valeurEncodeur1 != nbPulseVoulu1) 
    {
      ajustementMoteur(RIGHT, nbPulseVoulu1, i, valeurEncodeur1, &nbPulseTotal1, &derniereVitesseMoteur1);
    }

    moyenneEncodeur = (fabs(valeurEncodeur0) + fabs(valeurEncodeur1))/2;
    distanceParcourue = distanceParcourue + (moyenneEncodeur/NB_PULSE_TOUR) * pi*DIAMETRE_ROUE;
    Serial.print("\n Distance Voulu: ");
    Serial.print(distanceVoulue);
    Serial.print(" | Distance Parcourue: ");
    Serial.print(distanceParcourue);
    //Serial.println(distanceParcourue, 10); 
    i++;
  }
  
  MOTOR_SetSpeed(LEFT, 0); //Le robot s'arrete une fois que la distance est atteinte ou quand son bumper avant est frappé
  MOTOR_SetSpeed(RIGHT, 0);

  delay(500);
}

void ajustementMoteur(int coteMoteur, double nbPulseVoulu, double nbCycles, double valeurEncodeur, double *nbPulseTotal, double *derniereVitesseMoteur)
{ 
  double diffDistance = nbPulseVoulu - valeurEncodeur;
  (*nbPulseTotal) = (*nbPulseTotal) + valeurEncodeur;
  double correctionInteg = nbPulseVoulu * nbCycles - (*nbPulseTotal) ;
  MOTOR_SetSpeed(coteMoteur, (*derniereVitesseMoteur) + (diffDistance * 0.0001) + (correctionInteg * 0.000015));
  (*derniereVitesseMoteur) = (*derniereVitesseMoteur) + (diffDistance * 0.0001) + (correctionInteg * 0.000015);
}