### Project Name: **SMARTSTOP – Adaptive Braking and Notification System for Electric Scooters**

🚴‍♂️ **SMARTSTOP** is an innovative project designed to enhance the safety of electric scooters through adaptive braking and real-time notifications. The system uses various sensors (ultrasonic, obstacle, and distance sensors) to detect obstacles in the scooter's path and adjusts the braking accordingly, while also providing visual and audible alerts to the rider. The goal is to create a safer riding experience by integrating multiple safety mechanisms into one cohesive system.

### Demo Code 🖥️
Here are the prototype codes for the SMARTSTOP system:

#### **Prototype 1: Basic Obstacle Detection & Alert System**
This code reads data from obstacle and distance sensors to determine the proximity of obstacles. If an obstacle is detected, the system alerts the rider with visual indicators (LEDs) and an audible alarm (buzzer). The LCD displays the distance to the detected object.

```cpp
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Setup of LCD, sensors, and pins
LiquidCrystal_I2C lcd(0x3F, 16, 2); 
const int greenLedPin = 9;
const int redLedPin = 7;
const int buzzerPin = 5;
const int obstacleSensorPin = A0;
const int distanceSensorTrigPin = 4;
const int distanceSensorEchoPin = 3;
...
```

#### **Prototype 2: Advanced Safety Features with Servo, Vibration, and More**
This prototype builds upon the basic system and adds features like a servo to adjust the scooter's handlebars, vibration motor for haptic feedback, and additional alarms for varying levels of danger.

```cpp
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// Setup of LCD, sensors, and pins
LiquidCrystal_I2C lcd(0x3F, 16, 2);
Servo myServo;
const int greenLedPin = 8;
const int blueLedPin = 10;
const int redLedPin = 7;
...
```

#### **Prototype 3: Multi-Sensor Integration with Motor Control**
The third prototype integrates multiple ultrasonic sensors to detect obstacles from different directions and controls the scooter's motor based on the detected obstacles. The motor speed is adjusted dynamically based on the proximity of obstacles.

```cpp
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// Setup of sensors, motor, and LCD
LiquidCrystal_I2C lcd(0x3F, 16, 2);
Servo servo;
const int motorIn1 = 9;
const int motorIn2 = 10;
...
```

---

### File Organization 📂

Here’s how you can organize the files for the project:

1. **Project Folder:**
   Create a folder called `SMARTSTOP_Project`.

2. **Subfolders:**
   - `prototypes/` – This folder will contain all the source code files (`*.ino` for Arduino).
   - `docs/` – Use this folder to store documentation, diagrams, and other reference materials.
   - `assets/` – Store images, videos, or other media here (e.g., project pictures, video demonstrations).
   
3. **Upload to GitHub:**
   - Create a repository on GitHub and push the files.
   - Use descriptive commit messages, e.g., "Initial commit of SMARTSTOP prototype 1", "Added servo motor control", etc.

---

### How to Set Up 🚀

1. **Hardware:**
   - Connect the sensors, LEDs, servo, and motor to the corresponding pins as specified in the code.
   - Use a suitable power supply for the components (e.g., Arduino Uno or similar).
   
2. **Software:**
   - Download the [Arduino IDE](https://www.arduino.cc/en/software).
   - Open the project files in the IDE and upload the code to your Arduino board.

3. **Testing:**
   - Test the system on your electric scooter or a prototype setup.
   - Monitor the outputs on the LCD and ensure the alarms and indicators function correctly.

---

### Future Improvements 🔧

- **Bluetooth Integration**: Integrating Bluetooth for mobile notifications 📱.
- **AI for Object Recognition**: Use AI to better recognize different objects in the path 🤖.
- **Battery Saving Features**: Implement power-saving modes when the scooter is idle ⚡.

---

### License 📝
This project is open source under the MIT License.

---

Feel free to adjust the folder structure, commit messages, or any part of the description to better match your needs!
