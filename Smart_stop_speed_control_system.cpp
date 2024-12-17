#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// LCD 16×2
LiquidCrystal_I2C lcd(0x3F, 16, 2);

// Ультразвукові датчики
const int trigPin1 = 2;
const int echoPin1 = 3;
const int trigPin2 = 4;
const int echoPin2 = 5;
const int trigPin3 = 6;
const int echoPin3 = 7;

// Сервопривод
const int servoPin = 8;
Servo servo;

// Пасивний зумер
const int buzzerPin = 13;

// Кнопка
const int buttonPin = 12;

// Мотор
const int motorIn1 = 9;
const int motorIn2 = 10;
const int motorEn = 11;
const int potPin = A0;

// Глобальні змінні
bool systemEnabled = false;    // Стан системи
bool motorLocked = false;      // Чи заблоковано мотор
int currentMotorSpeed = 0;     // Поточна швидкість мотора
int minMotorSpeed = 100;       // Мінімальна швидкість
bool lastButtonState = LOW;    // Стан кнопки

// Для контролю перешкод
bool obstaclePreviouslyDetected = false; // Чи була перешкода раніше

void setup() {
    Serial.begin(9600);

    // Ініціалізація LCD
    Wire.begin();
    lcd.init();
    lcd.backlight();
    lcd.clear();
    lcd.print("Initializing...");
    delay(2000);
    lcd.clear();

    // Налаштування пінів
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

    // Ініціалізація сервопривода
    servo.attach(servoPin);
    servo.write(0); // Початкове положення
}

void loop() {
    // Перевірка кнопки
    bool currentButtonState = digitalRead(buttonPin);
    if (currentButtonState == LOW && lastButtonState == HIGH) {
        systemEnabled = !systemEnabled;
        if (systemEnabled) {
            lcd.clear();
            lcd.print("System: ON");
        } else {
            resetSystem();
        }
        delay(300); // Антидребезг
    }
    lastButtonState = currentButtonState;

    // Якщо система вимкнена
    if (!systemEnabled) return;

    // Вимірювання відстаней
    float distance1 = filterDistance(measureDistance(trigPin1, echoPin1));
    float distance2 = filterDistance(measureDistance(trigPin2, echoPin2));
    float distance3 = filterDistance(measureDistance(trigPin3, echoPin3));

    // Виведення на LCD
    lcd.setCursor(0, 0);
    lcd.print("D1:");
    lcd.print(distance1);
    lcd.print(" D2:");
    lcd.print(distance2);
    lcd.setCursor(0, 1);
    lcd.print("D3:");
    lcd.print(distance3);

    // Перевірка наявності перешкоди
    bool obstacleDetected = (distance1 < 40 || distance2 < 40 || distance3 < 40);

    // Обробка сигналів
    if (obstacleDetected && !obstaclePreviouslyDetected) {
        // Якщо перешкода з'явилася вперше
        signalObstacleDetected();
        handleObstacle(distance1, distance2, distance3);
    } else if (!obstacleDetected && obstaclePreviouslyDetected) {
        // Якщо перешкода зникла
        signalObstacleCleared();
        resetObstacle();
    }

    // Оновлюємо стан перешкоди
    obstaclePreviouslyDetected = obstacleDetected;

    // Якщо немає перешкод, працюємо з мотором
    if (!obstacleDetected) {
        handleMotor();
    }

    delay(50); // Основна затримка
}

// Сигналізація про виявлення перешкоди
void signalObstacleDetected() {
    lcd.clear();
    lcd.print("Obstacle Detected!");
    tone(buzzerPin, 1000, 200); // 200 мс сигналу
    delay(200);                 // Чекаємо завершення сигналу
}

// Сигналізація про зникнення перешкоди
void signalObstacleCleared() {
    lcd.clear();
    lcd.print("Obstacle Cleared!");
    tone(buzzerPin, 1200, 200); // 200 мс сигналу з вищою частотою
    delay(200);                 // Чекаємо завершення сигналу
}

// Обробка перешкод
void handleObstacle(float distance1, float distance2, float distance3) {
    motorLocked = true;

    if (distance1 < 40) { // Перешкода спереду
        lcd.clear();
        lcd.print("Obstacle Front!");
        servo.write(90);
        slowDownMotorTo(0);
    } else if (distance2 < 40 || distance3 < 40) { // Перешкода збоку
        lcd.clear();
        lcd.print("Obstacle Side!");
        servo.write(45);
        slowDownMotorTo(minMotorSpeed);
    }
}

// Скидання стану після перешкоди
void resetObstacle() {
    motorLocked = false;
    lcd.clear();
    lcd.print("All clear!");
    servo.write(0);
}

// Управління мотором
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

// Плавне гальмування мотора
void slowDownMotorTo(int targetSpeed) {
    for (int speed = currentMotorSpeed; speed >= targetSpeed; speed -= 5) {
        analogWrite(motorEn, speed);
        delay(100);
    }
    currentMotorSpeed = targetSpeed;
}

// Фільтрація відстані
float filterDistance(float distance) {
    if (distance < 2 || distance > 400) return 400; // Фільтруємо нереалістичні значення
    return distance;
}

// Вимірювання відстані
float measureDistance(int trigPin, int echoPin) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    long duration = pulseIn(echoPin, HIGH, 30000); // Таймаут 30 мс
    return (duration * 0.034) / 2;
}

// Скидання системи
void resetSystem() {
    lcd.clear();
    lcd.print("System: OFF");
    servo.write(0);
    digitalWrite(motorIn1, LOW);
    digitalWrite(motorIn2, LOW);
    analogWrite(motorEn, 0);
    motorLocked = false;
    obstaclePreviouslyDetected = false;
}
