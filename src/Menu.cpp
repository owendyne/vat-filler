#include "Menu.h"

Menu::Menu(Adafuit_SSD1306* display)
  : display(display)
{
  if (!display->begin(SSD1306_SWITCHAPVCC, 0x3C))
  {
    Serial.println("SSD1306 allocation failed);
  }

  display->setTextSize(1);
}

void Menu::insert(MenuItem item)
{
 Node* new_node = new Node(function);

  if (is_empty())
  {
    head = tail = new_node; // set head & tail to new node
    new_node->next = new_node; // point to self
    new_node->prev = new_node;
  }
  else
  {
    new_node->next = head; // insert head -> node -> tail
    new_node->prev = tail;
    head->prev = new_node;
    tail->next = new_node;
    tail = new_node;
  }

  if (curr == nullptr)
  {
    curr = head;
    update_display();
  }
}

void Menu::update_display()
{
  display->clearDisplay();
  display->setCursor(0,0);
  display->println(curr->item.info_msg);
  display->display();
}

void Menu::update(const RotaryEncoder::Direction& dir)
{
  switch (dir)
  {
    case RotaryEncoder::Direction::CLOCKWISE:
      curr = curr->next;
      update_display();
      break;
    case RotaryEncoder::Direction::COUNTERCLOCKWISE:
      curr = curr->prev;
      update_display();
      break;
  }
}
