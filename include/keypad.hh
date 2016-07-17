#ifndef KEYPAD_HH

#define KEYPAD_HH 1
#include <unordered_map>

class Keypad {
  std::unordered_map<char, bool> keymap{
    { '0', false }, { '1', false }, { '2', false }, { '3', false }, { '4', false },
    { '5', false }, { '6', false }, { '7', false }, { '8', false }, { '9', false },
    { 'A', false }, { 'B', false }, { 'C', false }, { 'D', false }, { 'E', false },
    { 'F', false }
  };

public:
  bool isPressed(const char keyCode);
  void pressedUp(const char keyCode);
  void pressedDown(const char keyCode);
};

#endif