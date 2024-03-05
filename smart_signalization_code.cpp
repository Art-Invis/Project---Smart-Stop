#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3F, 16, 2); // Адреса дисплею і розміри (16x2)

const int greenLedPin = 9;
const int redLedPin = 7;
const int buzzerPin = 5;
const int obstacleSensorPin = A0; // Використовуй A0 для аналогового піна
const int distanceSensorTrigPin = 4;
const int distanceSensorEchoPin = 3;

const float criticalDistance = 10; // Критична відстань в метрах
const int obstacleThreshold = 500; // Значення порогу для виявлення перешкоди

bool obstacleSoundPlayed = false; // Флаг для слідкування, чи вже відтворювався звук при виявленні перешкоди

unsigned long lastMillis = 0; // Змінна для відстеження часу
unsigned long buzzStartMillis = 0; // Час початку звукового сигналу

void setup() {
  pinMode(greenLedPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(obstacleSensorPin, INPUT);
  pinMode(distanceSensorTrigPin, OUTPUT);
  pinMode(distanceSensorEchoPin, INPUT);

  Wire.begin();  // Додано ініціалізацію I2C для використання LCD-дисплею

  lcd.init();  // Ініціалізація LCD-дисплею
  lcd.backlight();  // Увімкнення підсвічування

  lcd.begin(16, 2);
  lcd.print("  Safe move !");
}

void loop() {
  int obstacleValue = analogRead(obstacleSensorPin);

  if (obstacleValue < obstacleThreshold) {
    // Якщо виявлено перешкоду, вимірюємо відстань
    float distance = measureDistance();

    if (distance > criticalDistance) {
      displaySafeDistance(distance);
      activateGreenIndication();
      if (!obstacleSoundPlayed) {
        activateShortAlarm();
        obstacleSoundPlayed = true; // Встановлюємо флаг, що звук вже відтворений
      }
    } else {
      displayCriticalDistance(distance);
      activateRedIndication();
      activateCriticalAlarm();
      obstacleSoundPlayed = false; // Скидаємо флаг, щоб новий звук міг відтворитися
    }
  } else {
    // Якщо перешкоди немає, виконуємо інші дії
    activateGreenIndication();
    displaySafeMovementMessage();
    obstacleSoundPlayed = false; // Скидаємо флаг, щоб новий звук міг відтворитися
  }
}

float measureDistance() {
  digitalWrite(distanceSensorTrigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(distanceSensorTrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(distanceSensorTrigPin, LOW);

  float duration = pulseIn(distanceSensorEchoPin, HIGH);
  return duration * 0.0343 / 2; // Перерахунок відстані з часу
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
  tone(buzzerPin, 1200, 200); // Короткий звуковий сигнал на 200 мс
}

void activateCriticalAlarm() {
  tone(buzzerPin, 1000, 800); // Звуковий сигнал в критичних ситуаціях
}

void displaySafeMovementMessage() {
  lcd.clear();
  lcd.print("  Safe move !");
  delayWithTimer(500);
}

void delayWithTimer(unsigned long duration) {
  unsigned long currentMillis = millis();
  while (millis() - currentMillis < duration) {
    // Виконуємо інші дії, якщо потрібно
  }
}
