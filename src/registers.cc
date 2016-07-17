#include <string>
#include <cassert>
#include "registers.hh"

unsigned short& Registers::getSpecialRegister(std::string registerName) {
  return specialRegisters.at(registerName);
}

unsigned char& Registers::getDataRegister(std::string registerName) {
  return dataRegisters.at(registerName);
}