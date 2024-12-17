#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// LCD 
LiquidCrystal_I2C lcd(0x3F, 16, 2);

// ультразвукові датчики
const int trigPin1 = 2;
const int echoPin1 = 3;
const int trigPin2 = 4;
const int echoPin2 = 5;
const int trigPin3 = 6;
const int echoPin3 = 7;

const int servoPin = 8;
Servo servo;

const int buzzerPin = 13;

const int buttonPin = 12;

// мотор
const int motorIn1 = 9;
const int motorIn2 = 10;
const int motorEn = 11;
const int potPin = A0;

bool systemEnabled = false;
bool motorLocked = false;
int currentMotorSpeed = 0; 
int minMotorSpeed = 100; 
bool lastButtonState = LOW;

void setup() {
    Serial.begin(9600);

    Wire.begin();
    lcd.init();
    lcd.backlight();
    lcd.clear();
    lcd.print("Initializing...");
    delay(2000);
    lcd.clear();

    pinMode(trigPin1, OUTPUT);
    pinMode(echoPin1, INPUT);
    pinMode(trigPin2, OUTPUT);
    pinMode(echoPin2, INPUT);
    pinMode(trigPin3, OUTPUT);
    pinMode(echoPin3, INPUT);
    pinMode(buzzerPin, OUTPUT);
    pinMode(buttonPin, INPUT_PULLUP);
    pinMode(motorIn1, OUTPUT);
    pinMode(motorIn2, OUTPUT);
    pinMode(motorEn, OUTPUT);

    // сервопривід
    servo.attach(servoPin);
    servo.write(0);
}

void loop() {
    // кнопка
    bool currentButtonState = digitalRead(buttonPin);
    if (currentButtonState == LOW && lastButtonState == HIGH) {
        systemEnabled = !systemEnabled;
        if (systemEnabled) {
            lcd.clear();
            lcd.print("System: ON");
        } else {
            resetSystem();
        }
        delay(300); 
    }
    lastButtonState = currentButtonState;

    if (!systemEnabled) return;

    float distance1 = filterDistance(measureDistance(trigPin1, echoPin1));
    float distance2 = filterDistance(measureDistance(trigPin2, echoPin2));
    float distance3 = filterDistance(measureDistance(trigPin3, echoPin3));

    lcd.setCursor(0, 0);
    lcd.print("D1:");
    lcd.print(distance1);
    lcd.print(" D2:");
    lcd.print(distance2);
    lcd.setCursor(0, 1);
    lcd.print("D3:");
    lcd.print(distance3);

    if (distance1 < 40 || distance2 < 40 || distance3 < 40) {
        handleObstacle(distance1, distance2, distance3);
    } else {
        resetObstacle();
        handleMotor();
    }

    delay(100);
}

void handleObstacle(float distance1, float distance2, float distance3) {
    motorLocked = true;

    if (distance1 < 40) {
        lcd.clear();
        lcd.print("Obstacle Front!");
        servo.write(90);
        slowDownMotorTo(0);
    } else if (distance2 < 40 || distance3 < 40) {
        lcd.clear();
        lcd.print("Obstacle Side!");
        servo.write(45);
        slowDownMotorTo(minMotorSpeed);
    }
}

void resetObstacle() {
    if (!motorLocked) return;
    motorLocked = false;
    lcd.clear();
    lcd.print("Clear!");
    noTone(buzzerPin);
    servo.write(0);
}

// мотор
void handleMotor() {
    if (motorLocked) return;
    int potValue = analogRead(potPin);
    int motorSpeed = map(potValue, 0, 1023, 0, 255);

    if (motorSpeed == 0) {
        digitalWrite(motorIn1, LOW);
        digitalWrite(motorIn2, LOW);
        analogWrite(motorEn, 0);
    } else if (motorSpeed != currentMotorSpeed) {
        digitalWrite(motorIn1, HIGH);
        digitalWrite(motorIn2, LOW);
        analogWrite(motorEn, motorSpeed);
        currentMotorSpeed = motorSpeed;
    }
}

// скидання швидкості
void slowDownMotorTo(int targetSpeed) {
    for (int speed = currentMotorSpeed; speed >= targetSpeed; speed -= 5) {
        analogWrite(motorEn, speed);
        delay(100);
    }
    currentMotorSpeed = targetSpeed;
}

float filterDistance(float distance) {
    if (distance < 2 || distance > 400) return 400; 
    return distance;
}


float measureDistance(int trigPin, int echoPin) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    long duration = pulseIn(echoPin, HIGH, 30000); 
    return (duration * 0.034) / 2;
}

// Скидання системи
void resetSystem() {
    lcd.clear();
    lcd.print("System: OFF");
    noTone(buzzerPin);
    servo.write(0);
    digitalWrite(motorIn1, LOW);
    digitalWrite(motorIn2, LOW);
    analogWrite(motorEn, 0);
    motorLocked = false;
}
