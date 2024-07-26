#pragma once

#include "art_rle.h"

static unsigned menuid = 0;

struct MenuItem
{
  unsigned id;
  char* name;

  MenuItem* prev = nullptr;
  MenuItem* next = nullptr;
  const rle_t* art = nullptr;


  void (*action)(void) = nullptr; // function pointer to 

  MenuItem(const char* name, const rle_t* art) : name(name), art(art), id(menuid++) {}; // postfix static increment

};