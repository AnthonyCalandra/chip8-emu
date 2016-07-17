#include "stack.hh"

unsigned short& Stack::operator[](std::size_t index) {
  return frames[index];
}