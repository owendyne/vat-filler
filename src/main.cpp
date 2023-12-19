#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RotaryEncoder.h>

#include "Pins.h"
#include "Menu.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// objects;
Adafruit_SSD1306* display;
RotaryEncoder* encoder;

void encoder_int() { encoder->tick(); };

void test(Adafruit_SSD1306* display, RotaryEncoder* encoder)
{
  display->clearDisplay();
  display->setCursor(0,0);

  display->println("test program");
  display->display();

  delay(1000);
}


MenuItem<Adafuit_SSD1306*, RotaryEncoder*> item_a(test, "TEST");

void setup()
{
  Serial.begin(115200);

  attachInterrupt(Pins::RotaryEncoder::A, encoder_int, CHANGE);
  attachInterrupt(Pins::RotaryEncoder::B, encoder_int, CHANGE);


  display = new Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
  menu = new Menu(display);

  menu->insert(item_a);
}

void loop()
{
  const auto& dir = encoder->getDirection();
  menu->update(dir);
}
