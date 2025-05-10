/*
* Name: IR controlled car with autodriving and manual driving.
* Author: Dennis Jiang
* Date: 2025-05-10
* Description: This project controls a remote-controlled car using an IR remote and an ultrasonic sensor. 
* The car can be manually driven or switched to "smart forward" mode, where it moves forward and avoids obstacles automatically.
* A buzzer will make a sound when an obstacle is detected in smart mode.
*/

// Libraries
#include <IRremote.hpp>  

// Motor pins
#define IN1 7   // Forward right motor control
#define IN2 8   // Forward left motor control
#define IN3 9   // Backward right motor control
#define IN4 10  // Backward left motor control

// IR Receiver codes
#define IR_FORWARD     0x2200
#define IR_BACKWARD    0xC200
#define IR_LEFT        0xA800
#define IR_RIGHT       0x6200
#define IR_SMART_FWD   0x200

// Ultrasonic Sensor
#define TRIGGER_PIN 13
#define ECHO_PIN 12
#define OBSTACLE_THRESHOLD 20  // Distance in cm

// Buzzer
#define BUZZER_PIN 6

// Global variables
bool isSmartForward = false;
bool isAvoiding = false;
unsigned long lastObstacleCheck = 0;
const unsigned long obstacleCheckInterval = 300; 

unsigned long lastIRSignalTime = 0;
const unsigned long signalTimeout = 200;  // (optimal time to stop)

void setup() {
  Serial.begin(9600);
  
  // Motor pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  stopCar();

  // IR receiver
  IrReceiver.begin(2);

  // Ultrasonic sensor
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Buzzer pin
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW); 

  Serial.println("Car Ready");
}

// Main loop
void loop() {
  // Handle IR input
  if (IrReceiver.decode()) {
    lastIRSignalTime = millis(); // Reset timer on new signal
    handleIRCommand();
    IrReceiver.resume();
  }

  // Stop car if no signal (manual mode)
  if (!isSmartForward && (millis() - lastIRSignalTime > signalTimeout)) {
    stopCar();
  }

  // Smart mode 
  if (isSmartForward) {
    if (millis() - lastObstacleCheck > obstacleCheckInterval) {
      checkObstacle();
    }
  }
}

/*
* This function handles incoming IR commands and moves as it suppose to.
* Parameters: None
* Returns: void
*/
void handleIRCommand() {
  Serial.print("Received Command: 0x");
  Serial.println(IrReceiver.decodedIRData.command, HEX);

  switch (IrReceiver.decodedIRData.command) {
    case IR_FORWARD:
      if (!isSmartForward) moveForward();
      break;
    case IR_BACKWARD:
      stopSmartMode();
      moveBackward();
      break;
    case IR_LEFT:
      stopSmartMode();
      turnLeft();
      break;
    case IR_RIGHT:
      stopSmartMode();
      turnRight();
      break;
    case IR_SMART_FWD:
      toggleSmartMode();
      break;
  }
}

/*
* This function checks for obstacles using an ultrasonic sensor and avoids them if needed. It also turn the buzzer on if there is a obstacle. 
* Parameters: None
* Returns: void
*/
void checkObstacle() {
  lastObstacleCheck = millis();
  int distance = getDistance();

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  if (distance > 0 && distance < OBSTACLE_THRESHOLD) {
    if (!isAvoiding) {
      Serial.println("Obstacle detected - Avoiding");
      isAvoiding = true;
    }
    digitalWrite(BUZZER_PIN, HIGH); 
    turnRight();
  } else {
    if (isAvoiding) {
      Serial.println("Path clear - Resuming forward");
      isAvoiding = false;
    }
    digitalWrite(BUZZER_PIN, LOW); 
    moveForward();
  }
}

/*
* This function toggles smart forward mode on or off
* Parameters: None
* Returns: void
*/
void toggleSmartMode() {
  isSmartForward = !isSmartForward;
  if (isSmartForward) {
    Serial.println("Smart Mode ON");
    moveForward();
  } else {
    Serial.println("Smart Mode OFF");
    digitalWrite(BUZZER_PIN, LOW);
    stopCar();
  }
}

/*
* This function stops smart mode manually (for exampel if you want to move backwards suddenly when you are in smartmode)
* Parameters: None
* Returns: void
*/
void stopSmartMode() {
  if (isSmartForward) {
    isSmartForward = false;
    Serial.println("Smart Mode OFF");
    digitalWrite(BUZZER_PIN, LOW);
  }
}

/*
* This function moves the car forward by controlling the motor pins
* Parameters: None
* Returns: void
*/
void moveForward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, HIGH);
  Serial.println("Moving FORWARD");
}

/*
* This function moves the car backward by controlling the motor pins
* Parameters: None
* Returns: void
*/
void moveBackward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  Serial.println("Moving BACKWARD");
}

/*
* This function turns the car to the left by setting motor directions
* Parameters: None
* Returns: void
*/
void turnLeft() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  Serial.println("Turning LEFT");
}

/*
* This function turns the car to the right by setting motor directions
* Parameters: None
* Returns: void
*/
void turnRight() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  Serial.println("Turning RIGHT");
}

/*
* This function stops all movement by setting motor pins HIGH
* Parameters: None
* Returns: void
*/
void stopCar() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, HIGH);
  Serial.println("STOPPED");
}

/*
* This function measures the distance using an ultrasonic sensor
* Parameters: None (global TRIGGER_PIN and ECHO_PIN)
* Returns: Distance in centimeters as an integer
*/
int getDistance() {
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  return duration * 0.034 / 2;  // Convert time to distance in cm
}
