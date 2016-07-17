#include <vector>
#include <algorithm>
#include "memorystore.hh"

unsigned char& MemoryStore::operator[](std::size_t index) {
  return this->memory[index];
}

void MemoryStore::loadRom(std::vector<char>&& buffer) {
  auto it = this->memory.begin();
  std::advance(it, romLocation);
  std::copy(buffer.begin(), buffer.end(), it);
}

void MemoryStore::loadFont(std::array<unsigned char, 80>&& fontset) {
  auto it = this->memory.begin();
  std::copy(fontset.begin(), fontset.end(), it);
}