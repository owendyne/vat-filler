#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "Menu.h"
#include "MenuItem.h"
#include "art_rle.h"

#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDR 0x3C // i2c addr on display
Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET);

Menu* menu;
MenuItem* mspeed, *mcallibrate, *minfo;
rle_t* current_art = nullptr;

void render_rle_art(rle_t* art)
{
  uint8_t color, count;
  uint16_t x = 0;
  uint16_t y = 0;

  for (uint16_t i = 0; i < art->size; i++)
  {
    color = art->data[i] >> 7; 
    count = art->data[i] & 0x7F;

    for (uint8_t j = 0; j < count; j++)
    {
      display.drawPixel(x, y, color);
      x++;
      if (x >= OLED_WIDTH)
      {
        y++;
        x = 0;
      }
    }
  }
}

void buildMenu()
{
  // menu items
  mspeed = new MenuItem("Pump Speed", &fast_art);
  mcallibrate = new MenuItem("Callibrate Sensor", &gear_art);
  minfo = new MenuItem("About", &info_art);

  menu = new Menu;
  menu->insert(mspeed);
  menu->insert(mcallibrate);
  menu->insert(minfo);
}

void printArtAddress(const rle_t* art)
{
  Serial.print("art address: 0x");
  Serial.println((uint32_t)art, HEX);
}

void setup() {
  Serial.begin(115200);

  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  delay(1000);

  buildMenu();
}

MenuAction decodeCommand(char command, bool* success)
{
  *success = true;
  switch (command) {
    case 'L': return MenuAction::LEFT;
    case 'R': return MenuAction::RIGHT;
    case 'S': return MenuAction::SELECT;
    case 'B': return MenuAction::BACK;
    default:
      *success = false;
      return MenuAction::LEFT; // default or error value
  }
}

void processInput(bool* update_display)
{
  *update_display = false;
  if (Serial.available() > 0)  
  {
    // Read the incoming byte:
    char command = Serial.read();
    bool command_success = true;
    MenuAction action = decodeCommand(command, &command_success);

    // read out rest of buffer
    while (Serial.available() > 0) Serial.read();

    if (!command_success)
    {
      Serial.println("Unknown command!");
    }
    else 
    {
      bool nav_success = menu->navigate(action);
      if (!nav_success) Serial.println("Navigation Failed");
      else 
      {
        *update_display = true;
        Serial.println("Navigation Success");
      }
    }
  }
}

void loop() {
  bool update_display;
  processInput(&update_display);

  if (menu->curr->art != nullptr && menu->curr->art != current_art)
  {
    current_art = menu->curr->art;
  }

  if (current_art != nullptr)
  {
    display.clearDisplay();
    render_rle_art(current_art);
    display.display();
  }

  delay(1000);
}