#ifndef MEMORYSTORE_HH

#define MEMORYSTORE_HH 1
#include <array>
#include <vector>

const std::size_t numMemoryLocations = 0x1000;
const std::size_t romLocation = 0x200;
const std::size_t fontLocation = 0;

class MemoryStore {
  std::array<unsigned char, numMemoryLocations> memory{};

public:
  unsigned char& operator[](std::size_t index);
  void loadRom(std::vector<char>&& buffer);
  void loadFont(std::array<unsigned char, 80>&& fontset);
};

#endif