#include <Arduino.h>
#include <AccelStepper.h>

// Define pins for the L298N as constants for clarity
const int motorPin1 = 9;  // L298N Input 1
const int motorPin2 = 11; // L298N Input 2
const int motorPin3 = 6;  // L298N Input 3
const int motorPin4 = 5;  // L298N Input 4
const int enablePin1 = 10; // L298N Enable Pin 1
const int enablePin2 = 7;  // L298N Enable Pin 2

// Create a new instance of the AccelStepper class
AccelStepper stepper(AccelStepper::FULL4WIRE, motorPin1, motorPin2, motorPin3, motorPin4);

void setup() {
    Serial.begin(9600);

    pinMode(enablePin1, OUTPUT);
    digitalWrite(enablePin1, HIGH); // Ensure motor driver is enabled
    pinMode(enablePin2, OUTPUT);
    digitalWrite(enablePin2, HIGH); // Ensure motor driver is enabled

    // Initialise stepper settings
    stepper.setMaxSpeed(300);      // Set max speed of stepper (you can adjust this value)
    stepper.setAcceleration(50);   // Set acceleration of stepper (you can adjust this value)
    stepper.setSpeed(300);          // Set speed of stepper (you can adjust this value)

    stepper.moveTo(1000);          // Move to a large position to keep running
}

void loop() {
    // Check if stepper has reached its target position, if not keep running
    if (stepper.distanceToGo() == 0) {
        stepper.moveTo(-stepper.currentPosition());  // Reverse direction when target is reached
    }
    stepper.run();
}