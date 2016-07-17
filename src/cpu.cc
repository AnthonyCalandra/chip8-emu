#include <thread>
#include <iostream>
#include <chrono>
#include "registers.hh"
#include "memorystore.hh"
#include "stack.hh"
#include "instruction.hh"
#include "cpu.hh"

void performCpuCycle(Registers& registers, MemoryStore& memory, Stack& stack) {
  unsigned short& pc = registers.getSpecialRegister("PC");
  unsigned short opcode = memory[pc] << 8 | memory[pc + 1];
  // Find instruction by using first 4 bits of opcode (identifier).
  auto instIter = instructionTable.find(opcode & 0xF000);
  if (instIter == instructionTable.cend()) {
    throw std::string("Unknown instruction.");
  } else {
    (instIter->second)(ChipContext(opcode, registers, memory, stack));
  }

  unsigned short& delayTimer = registers.getSpecialRegister("DT");
  if (delayTimer > 0) {
    delayTimer--;
  }

  unsigned short& soundTimer = registers.getSpecialRegister("ST");
  if (soundTimer > 0) {
    if (soundTimer == 1) {
      std::cout << "BEEP" << '\a' << std::endl;
    }

    soundTimer--;
  }
}

void executeChipCpu(Registers registers, MemoryStore memory, Stack stack) {
  while (!quit) {
    performCpuCycle(registers, memory, stack);
    // Sleep for about 60 times a second.
    std::this_thread::sleep_for(std::chrono::milliseconds(15));
  }
}