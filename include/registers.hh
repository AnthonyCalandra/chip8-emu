#ifndef REGISTERS_HH

#define REGISTERS_HH 1
#include <string>
#include <unordered_map>
#include "memorystore.hh"

class Registers {
  std::unordered_map<std::string, unsigned short> specialRegisters{{
    // Program counter, index, delay timer, sound timer, stack pointer.
    { "PC", romLocation }, { "I", 0 }, { "DT", 0 }, { "ST", 0 }, { "SP", 0 },
  }};
  std::unordered_map<std::string, unsigned char> dataRegisters{{
    { "V0", 0 }, { "V1", 0 }, { "V2", 0 }, { "V3", 0 }, { "V4", 0 }, { "V5", 0 },
    { "V6", 0 }, { "V7", 0 }, { "V8", 0 }, { "V9", 0 }, { "V10", 0 }, { "V11", 0 },
    { "V12", 0 }, { "V13", 0 }, { "V14", 0 }, { "V15", 0 }
  }};

public:
  unsigned short& getSpecialRegister(std::string registerName);
  unsigned char& getDataRegister(std::string registerName);
};

#endif
