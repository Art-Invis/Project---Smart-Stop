#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3F, 16, 2); 

const int greenLedPin = 9;
const int redLedPin = 7;
const int buzzerPin = 5;
const int obstacleSensorPin = A0; 
const int distanceSensorTrigPin = 4;
const int distanceSensorEchoPin = 3;

const float criticalDistance = 10; 
const int obstacleThreshold = 500; 

bool obstacleSoundPlayed = false; 

unsigned long lastMillis = 0; 
unsigned long buzzStartMillis = 0; 

void setup() {
  pinMode(greenLedPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(obstacleSensorPin, INPUT);
  pinMode(distanceSensorTrigPin, OUTPUT);
  pinMode(distanceSensorEchoPin, INPUT);

  Wire.begin();  
  lcd.init();  
  lcd.backlight();  

  lcd.begin(16, 2);
  lcd.print("  Safe move !");
}

void loop() {
  int obstacleValue = analogRead(obstacleSensorPin);

  if (obstacleValue < obstacleThreshold) {
    
    float distance = measureDistance();

    if (distance > criticalDistance) {
      displaySafeDistance(distance);
      activateGreenIndication();
      if (!obstacleSoundPlayed) {
        activateShortAlarm();
        obstacleSoundPlayed = true; 
      }
    } else {
      displayCriticalDistance(distance);
      activateRedIndication();
      activateCriticalAlarm();
      obstacleSoundPlayed = false; 
    }
  } else {
    activateGreenIndication();
    displaySafeMovementMessage();
    obstacleSoundPlayed = false; 
  }
}

float measureDistance() {
  digitalWrite(distanceSensorTrigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(distanceSensorTrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(distanceSensorTrigPin, LOW);

  float duration = pulseIn(distanceSensorEchoPin, HIGH);
  return duration * 0.0343 / 2; 
}

void displaySafeDistance(float distance) {
  lcd.clear();
  lcd.print("Obstacle!");
  lcd.setCursor(0, 1);
  lcd.print("Distance: ");
  lcd.print(distance);
  lcd.print("sm");
  delayWithTimer(500);
}

void displayCriticalDistance(float distance) {
  lcd.clear();
  lcd.print("Warning! Danger");
  lcd.setCursor(0, 1);
  lcd.print("Distance: ");
  lcd.print(distance);
  lcd.print("sm");
  delayWithTimer(500);
}

void activateGreenIndication() {
  digitalWrite(greenLedPin, HIGH);
  digitalWrite(redLedPin, LOW);
}

void activateRedIndication() {
  digitalWrite(greenLedPin, LOW);
  digitalWrite(redLedPin, HIGH);
}

void activateShortAlarm() {
  tone(buzzerPin, 1200, 200);
}

void activateCriticalAlarm() {
  tone(buzzerPin, 1000, 800); 
}

void displaySafeMovementMessage() {
  lcd.clear();
  lcd.print("  Safe move !");
  delayWithTimer(500);
}

void delayWithTimer(unsigned long duration) {
  unsigned long currentMillis = millis();
  while (millis() - currentMillis < duration) {
    
  }
}
