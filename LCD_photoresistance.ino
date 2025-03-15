#include <Wire.h>
#include <LiquidCrystal.h>  // Utilisation de la bibliothèque LiquidCrystal
#include<LiquidCrystal_I2C.h>
#include<OneButton.h>
//LCD.I2C lcd(0x27,16,2);
//LiquidCrystal lcd(12, 11, 5, 4, 3, 2);  // Broches RS, E, D4, D5, D6, D7
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Adresse I2C 0x27 pour un LCD 16x2

const int joystickX = A1;      // Broche pour l'axe X du joystick
const int joystickY = A2;      // Broche pour l'axe Y du joystick
const int buttonPin = 2;       // Broche pour le bouton du joystick
const int ledPin = 8;          // Broche pour la LED (climatisation ou phares)
const int photoResistorPin = A0; // Broche pour la photoresistance
  
int currentPage = 1;           // Page actuelle affichée
unsigned long previousMillis = 0;
const long interval = 100;      // Intervalle pour la transmission série

void setup() {
  lcd.begin(16, 2);            // Initialisation de l'écran LCD
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
  
  // Affichage initial
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Nom: Nguepi Delphine"); // Remplacez par votre nom
  lcd.setCursor(0, 1);
 create();
  lcd.print("           71"); // Remplacez par les 2 derniers chiffres de votre numéro étudiant
  delay(3000); // Attendre 3 secondes
}
void create(){
byte deli[8]={
  0b11100,
  0b00100,
  0b00100,
  0b00100,
  0b00101,
  0b00001,
  0b00001,
  0b00001
  };
   lcd.createChar(0,deli);
}
void loop() {
  // Lecture des valeurs du joystick
  int xValue = analogRead(joystickX);
  int yValue = analogRead(joystickY);
  
  // Gestion de la climatisation ou des phares
  manageSystem();

  // Affichage sur LCD
  displayPage();

  // Transmission série
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    Serial.print("etd:2404071,x:"); 
    Serial.print(xValue); // Envoie la valeur de xValue
    Serial.print(",y:");
    Serial.print(yValue); // Envoie la valeur de yValue
    Serial.print(",sys:");
    Serial.println(digitalRead(ledPin) ? 1 : 0); // 1 si LED allumée, 0 sinon
  }

  // Gestion du bouton
  if (digitalRead(buttonPin) == LOW) {
    currentPage = (currentPage == 1) ? 2 : 1; // Basculer entre les pages
    delay(200); // Anti-rebond
  }
  
  delay(100); // Intervalle pour éviter une lecture trop rapide
  
}


void displayPage() {
  lcd.clear();
  if (currentPage == 1) {
    // Afficher la luminosité
int sensorValue = analogRead(photoResistorPin); // Lire la valeur de la photoresistance
    lcd.setCursor(0, 0);
    lcd.print(" Pct Lum: ");
    lcd.print(round(sensorValue/10.29)); // Afficher la valeur lue
    lcd.print("%");
     lcd.setCursor(0, 1);
    lcd.print(" Phares: ");
    lcd.print(digitalRead(ledPin) ? "ON" : "OFF");  //parceque j'ai un numéro impair
  } else {
    // Afficher la vitesse et la direction
    lcd.setCursor(0, 1);
    int speed = map(analogRead(joystickY), 0, 1023, -25, 120); // Vitesse de -25 à 120 km/h
 
    if(speed<0){
      lcd.print("Recul: ");
       lcd.print(speed);
             lcd.print("km/h");
      }else{
         lcd.print("Avance: ");
         lcd.print(speed);
             lcd.print("km/h");
        }
    lcd.setCursor(0, 0);
    int direction = map(analogRead(joystickX), 0, 1023, -90, 90); // Direction de -90° à +90°
    lcd.print("Direction : ");
    lcd.print(direction);
    lcd.print(direction < 0 ? " G" : " D"); // Afficher G ou D
  }
}

void manageSystem() {
  // Lecture de la luminosité (photoresistance)
  int value = analogRead(photoResistorPin); // Lire la valeur de la photoresistance

  if (value) { // Seuil pour activer la LED (ajuster selon vos besoins)
    digitalWrite(ledPin, HIGH); // Allumer la LED
  } else {
    digitalWrite(ledPin, LOW); // Éteindre la LED
  }
}
