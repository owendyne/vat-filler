import processing.serial.*;

PrintWriter output; // Declare the PrintWriter
Serial udSerial; // Declare the Serial object

void setup() {
  println(Serial.list()); // Print available serial ports
  // Initialize the serial communication; adjust the index as needed to select the correct port
  udSerial = new Serial(this, Serial.list()[1], 9600);
  // Initialize the PrintWriter to write to a file
  output = createWriter("/Users/owendyne/Documents/GitHub/vat-filler/dust_concentration.txt");
}

void draw() {
  if (udSerial.available() > 0) {
    String SenVal = udSerial.readStringUntil('\n'); // Read a string until a newline
    if (SenVal != null) {
      output.println(SenVal.trim()); // Write the sensor value to the file
      println(SenVal.trim()); // Print the sensor value to the console (optional)
    }
  }
}

void keyPressed() {
  output.flush(); // Flush the buffer
  output.close(); // Close the file
  exit(); // Exit the program
}
