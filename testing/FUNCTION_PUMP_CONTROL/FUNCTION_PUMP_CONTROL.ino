#include <Arduino.h>
#include <AccelStepper.h>

// Define pins for the L298N and buttons as constants for clarity
const int motorPin1 = 9;   // L298N Input 1
const int motorPin2 = 11;  // L298N Input 2
const int motorPin3 = 6;   // L298N Input 3
const int motorPin4 = 5;   // L298N Input 4
const int enablePin1 = 10; // L298N Enable Pin 1
const int enablePin2 = 7;  // L298N Enable Pin 2
const int buttonPin2 = 2;  // Button pin for forward direction
const int buttonPin3 = 3;  // Button pin for reverse direction

// Create a new instance of the AccelStepper class
AccelStepper stepper(AccelStepper::HALF4WIRE, motorPin1, motorPin2, motorPin3, motorPin4);

void setup() {
    Serial.begin(9600);
    pinMode(enablePin1, OUTPUT);
    pinMode(enablePin2, OUTPUT);
    pinMode(buttonPin2, INPUT_PULLUP);
    pinMode(buttonPin3, INPUT_PULLUP);

    digitalWrite(enablePin1, HIGH);
    digitalWrite(enablePin2, HIGH);

    stepper.setMaxSpeed(1000);      // Set max speed of stepper (adjustable)
    stepper.setAcceleration(500); // Set high acceleration for instant speed-up
}

void loop() {
    static unsigned long lastButtonPress = 0;
    unsigned long debounceDelay = 50;  // 50 milliseconds debounce period

    if (millis() - lastButtonPress > debounceDelay) {
        if (digitalRead(buttonPin2) == LOW) {
            startPump(true, 500);
            lastButtonPress = millis();
        } else if (digitalRead(buttonPin3) == LOW) {
            startPump(false, 500);
            lastButtonPress = millis();
        } else {
            gradualStop();
        }
    }
    stepper.run();
}

void startPump(bool direction, int finalSpeed) {
    stepper.setSpeed(direction ? finalSpeed : -finalSpeed);
    stepper.moveTo(direction ? 1000000000 : -1000000000);
}


void gradualStop() {
    static int lastSpeed = 0;  // Last non-zero speed recorded

    if (stepper.speed() != 0) {
        lastSpeed = stepper.speed();
        stepper.setSpeed(0); // Immediately set speed to 0 to trigger deceleration
        stepper.setAcceleration(500); // Increase deceleration rate for a smoother stop
    }

    if (stepper.distanceToGo() == 0) {
        stepper.stop();
        if (!stepper.isRunning()) {
            stepper.disableOutputs(); // Disable outputs to save power once stopped
        }
    } else if (stepper.distanceToGo() != 0 && lastSpeed != 0) {
        // If still moving or failed to stop, trigger a stop command
        stepper.moveTo(stepper.currentPosition()); // Set target to current position to stop movement
    }
}
