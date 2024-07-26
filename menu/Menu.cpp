#include "Menu.h"

bool Menu::navigate(MenuAction action) {
    bool success = true;

    switch (action) {
        case MenuAction::LEFT:
            if (curr == nullptr) success = false;
            else curr = curr->prev;
            break;
        case MenuAction::RIGHT:
            if (curr == nullptr) success = false;
            else curr = curr->next;
            break;
        case MenuAction::SELECT:
            if (curr->action == nullptr) success = false;
            else curr->action();
            break;
        case MenuAction::BACK:
            // Implement BACK action if necessary
            break;
        default:
            success = false;
            break;
    }

    return success;
}

void Menu::insert(MenuItem* item) {
    if (root == nullptr) {
        root = item;
        root->prev = root;
        root->next = root;
        curr = root;
    } else {
        MenuItem* tail = root->prev;
        tail->next = item;
        item->prev = tail;
        root->prev = item;
        item->next = root;
    }
}

void Menu::reset() {
    curr = root;
}