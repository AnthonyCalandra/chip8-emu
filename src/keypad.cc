#include <SDL2/SDL.h>
#include "keypad.hh"

bool Keypad::isPressed(const char keyCode) {
  auto keyState = this->keymap.find(keyCode);
  if (keyState == this->keymap.end()) {
    return false;
  } else {
    return keyState->second;
  }
}

void Keypad::pressedUp(const char keyCode) {
  this->keymap[keyCode] = false;
}

void Keypad::pressedDown(const char keyCode) {
  this->keymap[keyCode] = true;
}