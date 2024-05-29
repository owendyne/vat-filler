#include <Arduino.h>
#include <AccelStepper.h>

// Define motor and button pins
const int motorPin1 = 9;   // L298N Input 1
const int motorPin2 = 11;  // L298N Input 2
const int motorPin3 = 6;   // L298N Input 3
const int motorPin4 = 5;   // L298N Input 4
const int enablePin1 = 10; // L298N Enable Pin 1
const int enablePin2 = 7;  // L298N Enable Pin 2
const int buttonPinForward = 2; // Button pin for forward direction
const int buttonPinReverse = 3; // Button pin for reverse direction

// Create AccelStepper object
AccelStepper stepper(AccelStepper::FULL4WIRE, motorPin1, motorPin2, motorPin3, motorPin4);

//int maxSpeed = 850; // Maximum speed

int maxSpeed = 650; // Maximum speed

void setup() {
    Serial.begin(9600);
    pinMode(enablePin1, OUTPUT);
    pinMode(enablePin2, OUTPUT);
    pinMode(buttonPinForward, INPUT_PULLUP);
    pinMode(buttonPinReverse, INPUT_PULLUP);

    digitalWrite(enablePin1, LOW); // Initially disable the motor
    digitalWrite(enablePin2, LOW); // Initially disable the motor
    
    stepper.setMinPulseWidth(20); 
    stepper.setMaxSpeed(maxSpeed);      // Set max speed
    stepper.setAcceleration(400);       // Set acceleration
    //stepper.setAcceleration(300);       // Set acceleration

    pinMode(12, OUTPUT);
}

void loop() {
    bool forwardPressed = digitalRead(buttonPinForward) == LOW;
    bool reversePressed = digitalRead(buttonPinReverse) == LOW;

    if (forwardPressed) {
        digitalWrite(12, HIGH);
        digitalWrite(enablePin1, HIGH);
        digitalWrite(enablePin2, HIGH);
        stepper.setSpeed(maxSpeed);
        stepper.runSpeed();
    } else if (reversePressed) {
        digitalWrite(12, HIGH);
        digitalWrite(enablePin1, HIGH);
        digitalWrite(enablePin2, HIGH);
        stepper.setSpeed(-maxSpeed);
        stepper.runSpeed();
    } else {
        digitalWrite(12, LOW);
        stepper.setSpeed(0);    // Stop the motor immediately when no buttons are pressed
        stepper.stop();         // Stop the stepper motor as quickly as possible
        digitalWrite(enablePin1, LOW); // Disable the motor outputs
        digitalWrite(enablePin2, LOW); // Disable the motor outputs
    }
}
