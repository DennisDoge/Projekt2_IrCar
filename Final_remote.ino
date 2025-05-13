/*
* Name: IR_Controller_Joystick
* Author: Dennis Jiang
* Date: 2025-05-10
* Description: This project sends IR commands to control a remote car using a joystick module. 
* It reads analog input from the joystick and sends appropriate IR commands for movement, 
* including forward, backward, left, right, and smartmode.
*/

// Include Libraries
#include <IRremote.hpp>  

// Define IR LED pin
#define IR_LED_PIN 4  

// Joystick pin definitions
#define JOY_X A0         // Horizontal 
#define JOY_Y A1         // Vertical 
#define JOY_BTN 2        // Joystick button

// Define IR command values 
#define IR_FORWARD     0x2200
#define IR_BACKWARD    0xC200
#define IR_LEFT        0xA800
#define IR_RIGHT       0x6200
#define IR_SMART_FWD   0x200

void setup() {
  Serial.begin(9600);

  pinMode(JOY_BTN, INPUT_PULLUP);
  IrSender.begin(IR_LED_PIN);
}

/*
* This function sends an IR command multiple times for reliability
* Parameters: command (32-bit NEC command code)
* command - The IR command to send
* Returns: void
*/
void sendIR(uint32_t command) {
  for (int i = 0; i < 3; i++) {             
    IrSender.sendNEC(command, 32);          // Send NEC protocol command
    delay(40);                             
  }
}

/*  
* Main control loop - reads joystick and sends IR commands  
* Parameters: None  
* Returns: void  
*/  

void loop() {
  // Read joystick X and Y axes
  int x = analogRead(JOY_X);
  int y = analogRead(JOY_Y);

  // Read joystick button (active LOW)
  bool btn = !digitalRead(JOY_BTN);

  Serial.print("X: "); Serial.print(x);
  Serial.print(" | Y: "); Serial.println(y);

  // Detect and send movement commands based on joystick
  if (y > 700 && abs(x - 512) < 100) {
    sendIR(IR_FORWARD);         // Forward
  } else if (y < 300 && abs(x - 512) < 100) {
    sendIR(IR_BACKWARD);        // Backward
  } else if (x > 700) {
    sendIR(IR_RIGHT);           // Right
  } else if (x < 300) {
    sendIR(IR_LEFT);            // Left
  } else if (btn) {
    sendIR(IR_SMART_FWD);       // smart forward mode
  }

  delay(50);  
}
