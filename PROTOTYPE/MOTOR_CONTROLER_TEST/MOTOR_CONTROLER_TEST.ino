#include <AccelStepper.h>
#include "OLED_DISPLAY.h"

// Define pin connections
const int dirPin = 5;
const int stepPin = 4;
const int button1Pin = 2; // Button to spin clockwise (emptying)
const int button2Pin = 3; // Button to spin anticlockwise (filling)
const int sleepPin = 8; // Sleep pin for the motor driver

// Define motor interface type
#define motorInterfaceType 1

// Creates an instance
AccelStepper myStepper(motorInterfaceType, stepPin, dirPin);

// Variables to store the current direction and running state
volatile bool button1Pressed = false;
volatile bool button2Pressed = false;
volatile bool motorRunning = false;
volatile bool emergencyEmptying = false;
volatile bool emergencyFilling = false;

// Timer variables for non-blocking analog read
unsigned long previousMillis = 0;
const long readInterval = 100; // 100 milliseconds for 10 readings per second
const long averageInterval = 1000; // 1 second for averaging

// Variables for filtering sensor readings
unsigned long startTime = 0;
float sumA0 = 0, sumA1 = 0;
int countA0 = 0, countA1 = 0; // Separate counters for A0 and A1 readings

// Leaky integrator parameters
const float alpha = 0.99; // Smoothing factor for steady state
const float initialAlpha = 0.2; // Smoothing factor for startup phase
const int startupReadings = 80; // Number of readings in the startup phase

// Low-pass filter parameters
const float beta = 0.9; // Smoothing factor for the low-pass filter

// Filter states
float yA0 = 0;
float yA1 = 0;
float lpA0 = 0;
float lpA1 = 0;

// Variable to track the number of readings
int readingCount = 0;

// Variables for emergency filling condition
const int consecutiveReadingsRequired = 10 * 10; // 10 seconds * 10 readings per second
int consecutiveReadingsCount = 0;
float previousFilteredA0 = 0;

// Flag to alternate readings
bool readA0Next = true;

void setup() {

  delay(500);
  
  // Initialize Serial communication
  Serial.begin(9600);
  Serial.println("Vat Filler Prototype Test");

  // Set the minimum pulse width (in microseconds)
  myStepper.setMinPulseWidth(1); // 1 microsecond for the A4988 driver

  // Set the maximum speed
  myStepper.setMaxSpeed(5000); // Maximum speed in steps per second

  // Configure button pins as inputs with internal pull-up resistors
  pinMode(button1Pin, INPUT_PULLUP);
  pinMode(button2Pin, INPUT_PULLUP);
  pinMode(dirPin, OUTPUT);
  pinMode(sleepPin, OUTPUT);

  digitalWrite(sleepPin, LOW); // Put motor driver to sleep

  // Attach interrupts to the buttons
  attachInterrupt(digitalPinToInterrupt(button1Pin), button1ISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(button2Pin), button2ISR, CHANGE);

  setupOLED();

  // Perform sensor calibration to initialize filter states
  calibrateSensors();

  // Initialize the start time for sensor reading
  startTime = millis();
}

void loop() {
  unsigned long currentMillis = millis();

  if (readingCount > startupReadings) {
    if (emergencyEmptying) { // Emergency emptying state
      motorRunning = true;
      digitalWrite(sleepPin, HIGH); // Ensure motor driver is awake
      myStepper.enableOutputs();
      digitalWrite(dirPin, HIGH);
      myStepper.setSpeed(3000); // Set desired speed in steps per second
    } else if (emergencyFilling) { // Emergency filling state
      motorRunning = true;
      digitalWrite(sleepPin, HIGH); // Ensure motor driver is awake
      myStepper.enableOutputs();
      digitalWrite(dirPin, LOW);
      myStepper.setSpeed(-3000); // Set desired speed in steps per second
    } else if (button1Pressed) { // Manual emptying state
      motorRunning = true;
      digitalWrite(sleepPin, HIGH); // Ensure motor driver is awake
      myStepper.enableOutputs();
      digitalWrite(dirPin, HIGH);
      myStepper.setSpeed(3700); // Set desired speed in steps per second
    } else if (button2Pressed) { // Manual filling state
      motorRunning = true;
      digitalWrite(sleepPin, HIGH); // Ensure motor driver is awake
      myStepper.enableOutputs();
      digitalWrite(dirPin, LOW);
      myStepper.setSpeed(-3700); // Set desired speed in steps per second
    } else if (motorRunning) {
      motorRunning = false;
      myStepper.setSpeed(0);
      myStepper.runSpeed(); // Ensure motor stops running
      myStepper.disableOutputs();
      digitalWrite(sleepPin, LOW); // Put motor driver to sleep
    }
  }

  // Run the motor at the set speed
  myStepper.runSpeed();

  // Check if it's time to read analog pins
  if (currentMillis - previousMillis >= readInterval) {
    previousMillis = currentMillis;

    // Alternate reading A0 and A1
    float filteredA0, filteredA1;

    if (readA0Next) {
      filteredA0 = readFilteredAnalog(A0, &yA0, &lpA0);
      filteredA1 = lpA1; // Use previous value
      sumA0 += filteredA0;
      countA0++;
    } else {
      filteredA1 = readFilteredAnalog(A1, &yA1, &lpA1);
      filteredA0 = lpA0; // Use previous value
      sumA1 += filteredA1;
      countA1++;
    }

    readA0Next = !readA0Next; // Toggle the flag

    // Calculate the gradient of the filtered value of A0
    float gradientA0 = filteredA0 - previousFilteredA0;
    previousFilteredA0 = filteredA0;

    // Check if the gradient of A0 is close to zero
    if (abs(gradientA0) < 0.25 && filteredA0 < 1000) {
      consecutiveReadingsCount++;
      // Exit emergency filling state if the condition is met for the required time
      if (consecutiveReadingsCount >= consecutiveReadingsRequired) {
        emergencyFilling = false;
        emergencyEmptying = false;
        motorRunning = false;
        myStepper.setSpeed(0);
        myStepper.runSpeed();
        myStepper.disableOutputs();
        digitalWrite(sleepPin, LOW); // Put motor driver to sleep
      }
    } else {
      consecutiveReadingsCount = 0; // Reset the counter if the condition is not met
    }

    if (currentMillis - startTime >= averageInterval) { // 1 second has passed
      float averageA0 = sumA0 / countA0;
      float averageA1 = sumA1 / countA1;

      // Round the averages to the nearest integer
      int roundedA0 = round(averageA0);
      int roundedA1 = round(averageA1);

      Serial.print("Average Filtered A0: ");
      Serial.print(roundedA0);
      Serial.print("\tAverage Filtered A1: ");
      Serial.println(roundedA1);

      updateOLED(roundedA0, roundedA1);

      // Check for sensor error condition
      if (roundedA0 > roundedA1) {
        Serial.println("ERROR");
        calibrateSensors();
      } else {
        // Check for emergency filling condition
        if (roundedA0 > 1000 && !emergencyEmptying) {
          emergencyFilling = true;
          button1Pressed = false;
          button2Pressed = false;
        }

        // Check for emergency emptying condition
        if (roundedA1 < 1000) {
          emergencyEmptying = true;
          emergencyFilling = false;
          button2Pressed = false; // Ensure filling doesn't interfere
        } else if (emergencyEmptying && roundedA1 >= 1020) {
          emergencyEmptying = false;
          motorRunning = false;
          myStepper.setSpeed(0);
          myStepper.runSpeed();
          myStepper.disableOutputs();
          digitalWrite(sleepPin, LOW); // Put motor driver to sleep
        }

        // Check to exit emergency filling state
        if (emergencyFilling && roundedA1 < 1000) {
          emergencyFilling = false;
          emergencyEmptying = true; // Transition to emergency emptying
        }

        // Reset for the next cycle
        startTime = currentMillis;
        sumA0 = 0;
        sumA1 = 0;
        countA0 = 0;
        countA1 = 0;
      }
    }
  }
}


float readFilteredAnalog(int pin, float* integratorState, float* lowPassState) {
  float rawValue = analogRead(pin);

  // Apply leaky integrator filter
  float leakyValue = applyLeakyIntegrator(rawValue, integratorState);

  // Apply low-pass filter
  float filteredValue = applyLowPassFilter(leakyValue, lowPassState);

  return filteredValue;
}

// Interrupt service routine for button 1
void button1ISR() {
  button1Pressed = digitalRead(button1Pin) == LOW;
  if (button1Pressed) {
    button2Pressed = false; // Ensure only one button is active at a time
    emergencyFilling = false;
  }
}

// Interrupt service routine for button 2
void button2ISR() {
  if (analogRead(A1) >= 1020) {
    button2Pressed = digitalRead(button2Pin) == LOW;
    if (button2Pressed){
      button1Pressed = false; // Ensure only one button is active at a time
      emergencyFilling = false;
    }
  }
}

// Function to apply the leaky integrator filter
float applyLeakyIntegrator(float input, float* y) {
  // Use a higher alpha value during the startup phase
  float currentAlpha = (readingCount < startupReadings) ? initialAlpha : alpha;
  *y = currentAlpha * (*y) + (1.0 - currentAlpha) * input;
  readingCount++;
  return *y;
}

// Function to apply the low-pass filter
float applyLowPassFilter(float input, float* y) {
  *y = beta * input + (1.0 - beta) * (*y);
  return *y;
}

// Function to calibrate sensors
void calibrateSensors() {
  for (int i = 0; i < startupReadings; i++) {
    float rawA0 = analogRead(A0);
    delayMicroseconds(10); // Short non-blocking delay
    float rawA1 = analogRead(A1);
    yA0 = initialAlpha * yA0 + (1.0 - initialAlpha) * rawA0;
    yA1 = initialAlpha * yA1 + (1.0 - initialAlpha) * rawA1;
    delay(readInterval); // Wait for the read interval before next reading
  }
  // Initialize low-pass filter states
  lpA0 = yA0;
  lpA1 = yA1;
}
