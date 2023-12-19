#ifndef MENU_ITEM_H
#define MENU_ITEM_H

#include <Adafruit_SSD1306.h>
#include <RotaryEncoder.h>

template<typename ReturnType, typename... Args>
using FunctionPtrType = ReturnType (*)(Args...);


template<typename ReturnType, typename... Args>
struct MenuItem
{
public:
  FunctionPtrType<ReturnType, Args...> func;
  char* info_msg;

  MenuItem(FunctionPtrType<ReturnType, Args...> func, char* info_msg) : func(func), info_msg(info_msg) {};
};


#endif // MENU_ITEM_H
