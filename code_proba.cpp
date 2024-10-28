#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// LCD display configuration
LiquidCrystal_I2C lcd(0x3F, 16, 2);

// Pin definitions
const int greenLedPin = 8;
const int blueLedPin = 10;
const int redLedPin = 7;
const int buzzerPin = 5;
const int obstacleSensorPin = A0;
const int distanceSensorTrigPin = 4;
const int distanceSensorEchoPin = 3;
const int vibrationMotorPin = 6;
const int servoPin = 11;

// Critical distances (in cm)
const float criticalDistance = 20;
const float dangerDistance = 35;
const float warningDistance = 50;

// Threshold for obstacle sensor
const int obstacleThreshold = 500;

// State variables
bool obstacleSoundPlayed = false;
Servo myServo;

// Timers
unsigned long servoTimer = 0;
unsigned long buzzerTimer = 0;
bool servoActive = false;
bool buzzerActive = false;

void setup() {
    pinMode(greenLedPin, OUTPUT);
    pinMode(blueLedPin, OUTPUT);
    pinMode(redLedPin, OUTPUT);
    pinMode(buzzerPin, OUTPUT);
    pinMode(vibrationMotorPin, OUTPUT);
    pinMode(obstacleSensorPin, INPUT);
    pinMode(distanceSensorTrigPin, OUTPUT);
    pinMode(distanceSensorEchoPin, INPUT);

    myServo.attach(servoPin);
    myServo.write(0);

    Wire.begin();
    lcd.init();
    lcd.backlight();
    lcd.begin(16, 2);
    lcd.print("Safe move!");
}

void loop() {
    int obstacleValue = analogRead(obstacleSensorPin);
    if (obstacleValue < obstacleThreshold) {
        float distance = measureDistance();
        handleDistance(distance);
    } else {
        activateGreenIndication();
        displaySafeMovementMessage();
        obstacleSoundPlayed = false;
    }

    handleTimers();
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

void handleDistance(float distance) {
    if (distance > warningDistance) {
        displaySafeDistance(distance);
        activateGreenIndication();
        if (!obstacleSoundPlayed) {
            activateShortAlarm();
            obstacleSoundPlayed = true;
        }
    } else if (distance > dangerDistance) {
        displayWarningDistance(distance);
        activateYellowIndication();
        activateServo(100, 7000);
        activateVibrationMotor();
        if (!buzzerActive) {
            activateShortAlarm();
        }
    } else if (distance > criticalDistance) {
        displayCriticalDistance(distance);
        activateRedIndication();
        activateServo(180, 10000);
        activateBuzzer(1000, 7000);
    } else {
        displayCriticalDistance(distance);
        activateAllIndication();
        activateServo(180, 10000);
        activateBuzzer(1000, 7000);
    }
}

void activateGreenIndication() {
    digitalWrite(greenLedPin, HIGH);
    digitalWrite(blueLedPin, LOW);
    digitalWrite(redLedPin, LOW);
}

void activateYellowIndication() {
    digitalWrite(greenLedPin, HIGH);
    digitalWrite(blueLedPin, HIGH);
    digitalWrite(redLedPin, LOW);
}

void activateRedIndication() {
    digitalWrite(greenLedPin, LOW);
    digitalWrite(blueLedPin, LOW);
    digitalWrite(redLedPin, HIGH);
}

void activateAllIndication() {
    digitalWrite(greenLedPin, HIGH);
    digitalWrite(blueLedPin, HIGH);
    digitalWrite(redLedPin, HIGH);
}

void activateShortAlarm() {
    tone(buzzerPin, 1200, 200);
}

void activateBuzzer(int frequency, int duration) {
    tone(buzzerPin, frequency);
    buzzerActive = true;
    buzzerTimer = millis() + duration;
}

void activateVibrationMotor() {
    digitalWrite(vibrationMotorPin, HIGH);
    delay(200);
    digitalWrite(vibrationMotorPin, LOW);
}

void activateServo(int angle, int duration) {
    myServo.write(angle);
    servoActive = true;
    servoTimer = millis() + duration;
}

void handleTimers() {
    unsigned long currentMillis = millis();

    if (servoActive && currentMillis > servoTimer) {
        myServo.write(0);
        servoActive = false;
    }

    if (buzzerActive && currentMillis > buzzerTimer) {
        noTone(buzzerPin);
        buzzerActive = false;
    }
}

void displaySafeDistance(float distance) {
    lcd.clear();
    lcd.print("Obstacle!");
    lcd.setCursor(0, 1);
    lcd.print("Distance: ");
    lcd.print(distance);
    lcd.print("cm");
    delay(500);
}

void displayWarningDistance(float distance) {
    lcd.clear();
    lcd.print("Warning!");
    lcd.setCursor(0, 1);
    lcd.print("Distance: ");
    lcd.print(distance);
    lcd.print("cm");
    delay(500);
}

void displayCriticalDistance(float distance) {
    lcd.clear();
    lcd.print("Danger!");
    lcd.setCursor(0, 1);
    lcd.print("Distance: ");
    lcd.print(distance);
    lcd.print("cm");
    delay(500);
}

void displaySafeMovementMessage() {
    lcd.clear();
    lcd.print("Safe move!");
    delay(500);
}
