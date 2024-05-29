#include <AccelStepper.h>

// Define pin connections
const int dirPin = 2;
const int stepPin = 3;
const int button1Pin = 4; // Button to spin forward
const int button2Pin = 5; // Button to spin reverse

// Define motor interface type
#define motorInterfaceType 1
int Microsteps = 4; //Stepsize (1 for full steps, 2 for half steps, 4 for quarter steps

// Creates an instance
AccelStepper myStepper(motorInterfaceType, stepPin, dirPin);

int maxSpeed = 10000; // Maximum speed

void setup() {
  // Initialize Serial communication
  Serial.begin(9600);

  // Set the minimum pulse width (in microseconds)
  myStepper.setMinPulseWidth(2);

  // Set the maximum speed, acceleration factor,
  // initial speed and the target position
  myStepper.setMaxSpeed(maxSpeed);
  myStepper.setAcceleration(100); // Increased acceleration for smoother ramp-up

  // Configure button pins as inputs
  pinMode(button1Pin, INPUT_PULLUP); // Use internal pull-up resistor
  pinMode(button2Pin, INPUT_PULLUP); // Use internal pull-up resistor

  // Configure direction pin as output
  pinMode(dirPin, OUTPUT);
}

void loop() {
  // Check if data is available to read
  if (Serial.available() > 0) {
    // Read the incoming byte:
    String inputString = Serial.readStringUntil('\n');

    // Convert the string to an integer
    int newMaxSpeed = inputString.toInt();

    // Set the new maximum speed
    if (newMaxSpeed > 0) {
      maxSpeed = newMaxSpeed;
      myStepper.setMaxSpeed(maxSpeed);
      Serial.print("New max speed set to: ");
      Serial.println(maxSpeed);
    } else {
      Serial.println("Invalid input. Please enter a positive integer.");
    }
  }

  // Check button states
  bool button1State = digitalRead(button1Pin) == LOW; // Button 1 pressed
  bool button2State = digitalRead(button2Pin) == LOW; // Button 2 pressed

  if (button1State) {
    // Set direction to forward
    digitalWrite(dirPin, HIGH);
    // Set speed and move
    myStepper.moveTo(myStepper.currentPosition() + 10000); // Arbitrary large number to keep moving
  } else if (button2State) {
    // Set direction to reverse
    digitalWrite(dirPin, LOW);
    // Set speed and move
    myStepper.moveTo(myStepper.currentPosition() - 10000); // Arbitrary large number to keep moving
  } else {
    // Stop the motor by setting the target position to the current position
    myStepper.moveTo(myStepper.currentPosition());
  }

  // Run the motor to the set position
  myStepper.run();
}