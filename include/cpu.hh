#ifndef CPU_HH

#define CPU_HH 1

#include <atomic>
#include "registers.hh"
#include "memorystore.hh"
#include "stack.hh"

extern std::atomic_bool quit;
void performCpuCycle(Registers& registers, MemoryStore& memory, Stack& stack);
void executeChipCpu(Registers registers, MemoryStore memory, Stack stack);

#endif