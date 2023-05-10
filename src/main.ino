#include <Servo.h>
#include <Adafruit_LiquidCrystal.h>


// CONSTANTES ############################################

// Ecran LCD
Adafruit_LiquidCrystal lcdScreen_1(0);

// Servo moteurs : Rotation des panneaux solaires
Servo servoH;
const int pwmServoH =  9;
const int servoHLimNeu = 90;
const int servoHLimMin = 45;
const int servoHLimMax = 135;
Servo servoV;
const int pwmServoV =  10;
const int servoVLimNeu = 90;
const int servoVLimMin = 45;
const int servoVLimMax = 135;

// Photorésistances : Rotation des panneaux solaires
const int ldrH = A4; // Haut
const int ldrB = A5; // Bas 
const int ldrG = A2; // Gauche
const int ldrD = A3; // Droite
const int PRMax = 310;
const int PRMin = 1;

// Capteur de Température : Température ambiante
const int inTemperature = A0;

// Humidité : Humidité du sol
const int inHumidity = A1;
const int humidityMax = 876.0;

// Pompe à eau
const int pwmWaterPump =  11;

// Paramètre du système
const float LIMIT_LOW_HUMIDITY = 30.;
const float LIMIT_HIGH_TEMPERATURE = 35.;
bool launchPumpManually = false;


// FONCTIONS #############################################

void print_start_setup()
{
  lcdScreen_1.setCursor(0, 0);
  lcdScreen_1.print("Lancement du");
  lcdScreen_1.setCursor(0, 1);
  lcdScreen_1.print("systeme en cours");
  delay(1000);
}

void print_stop_setup()
{
  lcdScreen_1.clear();
  lcdScreen_1.print("Configuration");
  lcdScreen_1.setCursor(0, 1);
  lcdScreen_1.print("terminee");
  delay(1000);
  lcdScreen_1.clear();
  lcdScreen_1.print("En marche");
  delay(1000);
}

void setup()
{ 
  Serial.begin(9600);
  lcdScreen_1.begin(16, 2);
  print_start_setup();
  servoV.attach(pwmServoV);
  servoH.attach(pwmServoH);
  servoV.write(servoVLimNeu);
  servoH.write(servoHLimNeu);
  pinMode(pwmWaterPump, OUTPUT);
  print_stop_setup();
}

// Température en degrés celsius
float get_temperature_in_C()
{  
  float analog = analogRead(inTemperature);
  float digital = analog * 5./1024;
  return round((digital - 0.5) * 100);
}

// Humidité du sol en %
float get_humidity_in_percentage()
{
  float analog = analogRead(inHumidity);
  return analog * 100 / humidityMax;
}

// Afficher la température et l'humidité sur l'ecran LCD
void show_temp_humidity_in_lcd(float t, float h)
{
  lcdScreen_1.clear();
  lcdScreen_1.print("Temp : ");
  lcdScreen_1.print(int(t));
  lcdScreen_1.print(" C");
  lcdScreen_1.setCursor(0, 1);
  lcdScreen_1.print("Humidite : ");
  lcdScreen_1.print(int(h));
  lcdScreen_1.print(" %");
}

// Rotation des panneaux solaires en fonction
// de la position du soleil
void update_panels_positions()
{
  int VldrH = analogRead(ldrH);
  int VldrB = analogRead(ldrB);
  int VldrG = analogRead(ldrG);
  int VldrD = analogRead(ldrD);
  // Verticalement
  if (VldrH > VldrB) servoV.write(servoVLimMin);
  else if(VldrH < VldrB) servoV.write(servoVLimMax);
  else servoV.write(servoVLimNeu);
  // Horizontalement
  if (VldrG > VldrD) servoH.write(servoHLimMax);
  else if(VldrG < VldrD) servoH.write(servoHLimMin);
  else servoH.write(servoHLimNeu);
  delay(1000);
}

// Allumer/Eteindre la pompe
// En fonction de l'état du bouton poussoir
// et de la température/humidité ambiantes
void update_waterPump_status()
{
  int buttonState = digitalRead(2);
  if(buttonState == HIGH){
    if(launchPumpManually == true){
      lcdScreen_1.clear();
      lcdScreen_1.print("Mode manuel  : ");
      lcdScreen_1.setCursor(0, 1);
      lcdScreen_1.print("DESACTIVE");
      launchPumpManually = false;
      delay(2000);
    }else{
      lcdScreen_1.clear();
      lcdScreen_1.print("Mode manuel  : ");
      lcdScreen_1.setCursor(0, 1);
      lcdScreen_1.print("ACTIVE");
      launchPumpManually = true;
      delay(2000);
    }
  }  
  float temperature = get_temperature_in_C();
  float humidity = get_humidity_in_percentage();
  if (launchPumpManually || (( temperature >=  LIMIT_HIGH_TEMPERATURE) && (humidity <= LIMIT_LOW_HUMIDITY))){
    if(digitalRead(pwmWaterPump) == LOW){
      show_temp_humidity_in_lcd(temperature, humidity);
      delay(2000);
      lcdScreen_1.clear();
      lcdScreen_1.print("Arrosage en");
      lcdScreen_1.setCursor(0, 1);
      lcdScreen_1.print("cours ...");
    }
    digitalWrite(pwmWaterPump, HIGH);
  }else{
    if(digitalRead(pwmWaterPump) == HIGH)
    {
      lcdScreen_1.clear();
      lcdScreen_1.print("Arret de");
      lcdScreen_1.setCursor(0, 1);
      lcdScreen_1.print("l'arrosage");
      delay(2000);
      show_temp_humidity_in_lcd(temperature, humidity);
      delay(2000);
      lcdScreen_1.clear();
    }
    digitalWrite(pwmWaterPump, LOW);
  }
}


// MAIN ##################################################

void loop()
{
  update_panels_positions();
  update_waterPump_status();
  delay(100);
}