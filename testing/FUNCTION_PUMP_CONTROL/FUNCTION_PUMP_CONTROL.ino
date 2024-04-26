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

// Debouncing variables
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 100;    // the debounce time; increase if the output flickers
bool lastForwardState = HIGH;        // the previous reading from the input pin
bool lastReverseState = HIGH;        // the previous reading from the input pin
bool forwardPressed = false;
bool reversePressed = false;

void setup() {
    Serial.begin(9600);
    pinMode(enablePin1, OUTPUT);
    pinMode(enablePin2, OUTPUT);
    pinMode(buttonPinForward, INPUT_PULLUP);
    pinMode(buttonPinReverse, INPUT_PULLUP);

    digitalWrite(enablePin1, LOW); // Disable the motor
    digitalWrite(enablePin2, LOW); // Disable the motor

    stepper.setMaxSpeed(4000);      // Set max speed
    stepper.setAcceleration(200);   // Set acceleration
    stepper.setCurrentPosition(0);
}

void loop() {
    int readingForward = digitalRead(buttonPinForward);
    int readingReverse = digitalRead(buttonPinReverse);

    // Check if the switch state has changed
    if (readingForward != lastForwardState) {
        lastDebounceTime = millis();
    }
    if (readingReverse != lastReverseState) {
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelay) {
        // Update the actual button state if stable for longer than the debounceDelay
        forwardPressed = readingForward == LOW;
        reversePressed = readingReverse == LOW;
    }

    // Save the reading
    lastForwardState = readingForward;
    lastReverseState = readingReverse;

    // Motor control logic
    if (forwardPressed && !reversePressed) {
        stepper.enableOutputs();
        digitalWrite(enablePin1, HIGH); // Enable the motor to move
        digitalWrite(enablePin2, HIGH); // Enable the motor to move
        stepper.setSpeed(700);         // Set speed for forward movement
        stepper.moveTo(1000000);  
        stepper.run();                 // Move stepper at set speed
    } else if (reversePressed && !forwardPressed) {
        stepper.enableOutputs();
        digitalWrite(enablePin1, HIGH); // Enable the motor to move
        digitalWrite(enablePin2, HIGH); // Enable the motor to move
        stepper.setSpeed(-700);        // Set speed for reverse movement
        stepper.moveTo(-1000000);
        stepper.run();                 // Move stepper at set speed
    } else {
        stepper.setSpeed(0);           // No buttons pressed, stop the motor
        stepper.stop();                // Ensure stepper is stopped
        stepper.disableOutputs();
        digitalWrite(enablePin1, LOW); // Disable the motor
        digitalWrite(enablePin2, LOW); // Disable the motor
    }
}
