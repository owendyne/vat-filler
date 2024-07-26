#pragma once

#include "MenuItem.h"

enum class MenuAction { LEFT, RIGHT, SELECT, BACK };

struct Menu
{
    MenuItem* root = nullptr;
    MenuItem* curr = nullptr;

    bool navigate(MenuAction action); // returns success
    void insert(MenuItem* item); // insert menu item after first (or as first)
    void reset(); // sets curr to root
};