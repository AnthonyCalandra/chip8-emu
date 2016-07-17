#ifndef STACK_HH

#define STACK_HH 1
#include <array>

const std::size_t maxStackFrames = 16;

class Stack {
  std::array<unsigned short, maxStackFrames> frames{};

public:
  unsigned short& operator[](std::size_t index);
};

#endif