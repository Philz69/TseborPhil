#include <Arduino.h>
#include <LibRobUS.h>
float lastMotorSpeed;
const double pi = 3.1415926535897932384626433832795028841;
double distance;
double encoderVal;
double const diametreRoue = 0.077;
double correctionMoteurGauche = 1.048;
double correctionMoteurDroit = 1;

void avancerDistance(double distanceVoulue, double vitesseVoulue)
{
  int delaiMesure = 50;
  int nbPulseVoulu = vitesseVoulue * delaiMesure / ((1/3200) * pi*diametreRoue); 
  MOTOR_SetSpeed(0, 0.25 * correctionMoteurGauche);
  MOTOR_SetSpeed(1, 0.25 * correctionMoteurDroit);
  double derniereVitesseMoteur0 = 0.25;
  double derniereVitesseMoteur1 = 0.25;
  
  double diffDistance;
  double distanceParcourue = 0;
  while( distanceParcourue < (distanceVoulue - 0.05) && !ROBUS_IsBumper(2) )
  {
    ENCODER_ReadReset(0);
    ENCODER_ReadReset(1);
    delay(delaiMesure);
    double valeurEncodeur0 = ENCODER_Read(0);
    double valeurEncodeur1 = ENCODER_Read(1);
    if (valeurEncodeur0 != nbPulseVoulu) 
    {
      diffDistance = 350 - valeurEncodeur0;
      MOTOR_SetSpeed(0, derniereVitesseMoteur0 + (diffDistance * 0.0005));
      derniereVitesseMoteur0 = derniereVitesseMoteur0 + (diffDistance * 0.0005);
    }
    if (valeurEncodeur1 != nbPulseVoulu) 
    {
      diffDistance = 350 - valeurEncodeur1;
      MOTOR_SetSpeed(1, derniereVitesseMoteur1 + (diffDistance * 0.0005));
      derniereVitesseMoteur1 = derniereVitesseMoteur1 + (diffDistance * 0.0005);
    }
    distanceParcourue = distanceParcourue + (((valeurEncodeur0 + valeurEncodeur1)/2)/3200) * pi*diametreRoue;
    Serial.println("Moteur Gauche:");
    Serial.println(derniereVitesseMoteur0);
    Serial.println("Moteur Droit:");
    Serial.println(derniereVitesseMoteur1);




  }
  MOTOR_SetSpeed(0, 0);
  MOTOR_SetSpeed(1, 0);
  

    
  

}

void setup()
{
  BoardInit();
  Serial.begin(9600);
    delay(1500);


  
  


  //delay(800);

  //while (ENCODER_Read(0) < 10000 && ENCODER_Read(1) < 10000)
  //{
    /* code */
  //}
  
  //delay(1000);
  //MOTOR_SetSpeed(0, 0);
  //MOTOR_SetSpeed(1, 0);


}

void loop() 
{
  if (ROBUS_IsBumper(3))
  {
     avancerDistance(5, 0.30);
  }
  
}






