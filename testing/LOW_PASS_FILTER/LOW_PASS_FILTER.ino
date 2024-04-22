volatile bool button2Pressed = false;
volatile bool button3Pressed = false;

void setup() {
  Serial.begin(9600);
  pinMode(8, OUTPUT);  // LED for button 2
  pinMode(9, OUTPUT);  // LED for button 3
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);

  // Attach interrupts for both buttons, triggering on change to handle both press and release
  attachInterrupt(digitalPinToInterrupt(2), handleButton2, CHANGE);
  attachInterrupt(digitalPinToInterrupt(3), handleButton3, CHANGE);
}

void loop() {

  static unsigned long lastToggleTime2 = 0;
  static unsigned long lastToggleTime3 = 0;
  static bool ledState2 = LOW;
  static bool ledState3 = LOW;

  // Handle flashing for button 2
  if (button2Pressed) {
    if (millis() - lastToggleTime2 >= 100) {
      ledState2 = !ledState2;
      digitalWrite(8, ledState2);
      lastToggleTime2 = millis();
    }
  } else {
    digitalWrite(8, LOW);  // Ensure LED is off when not pressing
  }

  // Handle flashing for button 3 without blocking
  if (button3Pressed) {
    if (millis() - lastToggleTime3 >= 100) {
      ledState3 = !ledState3;
      digitalWrite(9, ledState3);
      lastToggleTime3 = millis();
    }
  } else {
    digitalWrite(9, LOW);  // Ensure LED is off when not pressing
  }

  // Code for handling sensor readings and time-averaged filtering
  static unsigned long startTime = millis();
  static float sumA0 = 0, sumA1 = 0;
  static int count = 0;

  float filteredA0 = getLowPassFilteredReading(A0);
  float filteredA1 = getLowPassFilteredReading(A1);

  sumA0 += filteredA0;
  sumA1 += filteredA1;
  count++;

  if (millis() - startTime >= 1000) {  // 1 second has passed
    float averageA0 = sumA0 / count;
    float averageA1 = sumA1 / count;

    Serial.print("Average Filtered A0: ");
    Serial.print(averageA0);
    Serial.print("\tAverage Filtered A1: ");
    Serial.print(averageA1);

    if(averageA0 < 800 && averageA1 < 800){
      Serial.print("\tResin level: OVERFLOW\n");
    }
    else if(averageA0 > 800 && averageA1 < 800){
      Serial.print("\tResin level: GOOD\n");
    }
    else if(averageA0 > 800 && averageA1 > 800){
      Serial.print("\tResin level: EMPTY\n");
    }
    else{
      Serial.print("\tResin level: ERROR\n");
    }

    // Reset for the next cycle
    startTime = millis();
    sumA0 = 0;
    sumA1 = 0;
    count = 0;
  }
}

float getLowPassFilteredReading(int pin) {
  static float previousFilteredValues[2] = {0, 0};  // Holds the previous filtered values for each pin
  int index = (pin == A0) ? 0 : 1;  // Determine the index based on the pin
  float alpha = 0.7;  // Smoothing factor, adjust as needed
  float currentReading = analogRead(pin);  // Read the current value

  // Apply the low-pass filter equation
  float filteredValue = alpha * currentReading + (1 - alpha) * previousFilteredValues[index];

  // Update the previous filtered value for the next iteration
  previousFilteredValues[index] = filteredValue;

  return filteredValue;  // Return the filtered value
}

void handleButton2() {
  button2Pressed = digitalRead(2) == LOW;  // Update the state based on the button
}

void handleButton3() {
  button3Pressed = digitalRead(3) == LOW;  // Update the state based on the button
}


void togglePump(bool state){
}