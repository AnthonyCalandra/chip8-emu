#ifndef INSTRUCTION_HH

#define INSTRUCTION_HH 1
#include <unordered_map>
#include <random>
#include <future>
#include <atomic>
#include "sdlutils.hh"
#include "keypad.hh"

extern std::atomic_bool keySent;
extern std::future<unsigned char> keyPressFuture;
extern std::promise<unsigned char> keyPressPromise;
struct ChipContext {
  unsigned short opcode;
  Registers& registers;
  MemoryStore& memory;
  Stack& stack;
  ChipContext(unsigned short opcode, Registers& registers, MemoryStore& memory, Stack& stack) :
    opcode(opcode), registers(registers), memory(memory), stack(stack) {};
};

using InstructionFn = std::function<void(const ChipContext& context)>;
std::unordered_map<unsigned short, InstructionFn> instructionTable({
  {
    0, // 0NNN - unused; 00E0 - clears the screen; 00EE - returns from a subroutine.
    [](const ChipContext& context) {
      unsigned short& pc = context.registers.getSpecialRegister("PC");
      switch (context.opcode & 0x000F) {
        case 0: {
          buffer.clear();
          pc += 2;
          break;
        }

        case 0xE: {
          // Return to the address we previously were and add 2 so we don't execute the same
          // instruction again.
          unsigned short& sp = context.registers.getSpecialRegister("SP");
          pc = context.stack[--sp] + 2;
          break;
        }
      }
    }
  }, {
    0x1000, // 1NNN - jumps to address NN.
    [](const ChipContext& context) {
      unsigned short& pc = context.registers.getSpecialRegister("PC");
      pc = context.opcode & 0x0FFF;
    }
  }, {
    0x2000, // 2NNN - calls subroutine at address NNN.
    [](const ChipContext& context) {
      // Store the location we will return to on the stack.
      unsigned short& pc = context.registers.getSpecialRegister("PC");
      unsigned short& sp = context.registers.getSpecialRegister("SP");
      context.stack[sp++] = pc;
      pc = context.opcode & 0x0FFF;
    }
  }, {
    0x3000, // 3XNN - skips the next instruction if VX == NN.
    [](const ChipContext& context) {
      unsigned short& pc = context.registers.getSpecialRegister("PC");
      std::string registerNameX = "V" + std::to_string((context.opcode & 0x0F00) >> 8);
      unsigned char& vx = context.registers.getDataRegister(registerNameX);
      if (vx == (context.opcode & 0x00FF)) {
        pc += 2;
      }

      pc += 2;
    }
  }, {
    0x4000, // 4XNN - skips the next instruction if VX != NN.
    [](const ChipContext& context) {
      unsigned short& pc = context.registers.getSpecialRegister("PC");
      std::string registerNameX = "V" + std::to_string((context.opcode & 0x0F00) >> 8);
      unsigned char& vx = context.registers.getDataRegister(registerNameX);
      if (vx != (context.opcode & 0x00FF)) {
        pc += 2;
      }

      pc += 2;
    }
  }, {
    0x5000, // 5XY0 - skips the next instruction if VX == VY.
    [](const ChipContext& context) {
      unsigned short& pc = context.registers.getSpecialRegister("PC");
      std::string registerNameX = "V" + std::to_string((context.opcode & 0x0F00) >> 8);
      std::string registerNameY = "V" + std::to_string((context.opcode & 0x00F0) >> 4);
      unsigned char& vx = context.registers.getDataRegister(registerNameX);
      unsigned char& vy = context.registers.getDataRegister(registerNameY);
      if (vx == vy) {
        pc += 2;
      }

      pc += 2;
    }
  }, {
    0x6000, // 6XNN - sets VX to NN.
    [](const ChipContext& context) {
      unsigned short& pc = context.registers.getSpecialRegister("PC");
      std::string registerNameX = "V" + std::to_string((context.opcode & 0x0F00) >> 8);
      unsigned char& vx = context.registers.getDataRegister(registerNameX);
      vx = context.opcode & 0x00FF;
      pc += 2;
    }
  }, {
    0x7000, // 7XNN - adds NN to VX.
    [](const ChipContext& context) {
      unsigned short& pc = context.registers.getSpecialRegister("PC");
      std::string registerNameX = "V" + std::to_string((context.opcode & 0x0F00) >> 8);
      unsigned char& vx = context.registers.getDataRegister(registerNameX);
      unsigned char val = (context.opcode & 0x00FF) + vx;
      if (val > 0xFF) {
        val -= 0x100;
      }

      vx = val;
      pc += 2;
    }
  }, {
    /*
     * 8XY0 - sets VX to the value of VY.
     * 8XY1 - sets VX to VX OR VY.
     * 8XY2 - sets VX to VX AND VY.
     * 8XY3 - sets VX to VX XOR VY.
     * 8XY4 - adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.
     * 8XY5 - VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when
     *   there isn't.
     * 8XY6 - shifts VX right by one. VF is set to the value of the least significant bit
     *   of VX before the shift.
     * 8XY7 - sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when
     *   there isn't.
     * 8XYE - shifts VX left by one. VF is set to the value of the most significant bit of
     *   VX before the shift.
     */
    0x8000,
    [](const ChipContext& context) {
      unsigned short& pc = context.registers.getSpecialRegister("PC");
      std::string registerNameX = "V" + std::to_string((context.opcode & 0x0F00) >> 8);
      std::string registerNameY = "V" + std::to_string((context.opcode & 0x00F0) >> 4);
      unsigned char& vx = context.registers.getDataRegister(registerNameX);
      unsigned char& vy = context.registers.getDataRegister(registerNameY);
      unsigned char& vf = context.registers.getDataRegister("V15");
      switch (context.opcode & 0x000F) {
        case 0: {
          vx = vy;
          break;
        }

        case 1: {
          vx |= vy;
          break;
        }

        case 2: {
          vx &= vy;
          break;
        }

        case 3: {
          vx ^= vy;
          break;
        }

        case 4: {
          vx += vy;
          if (vx > 0xFF) {
            vf = 1;
            vx -= 0x100;
          } else {
            vf = 0;
          }

          break;
        }

        case 5: {
          if (vx > vy) {
            vf = 1;
          } else {
            vf = 0;
          }

          vx -= vy;
          if (vx < 0) {
            vx += 0x100;
          }

          break;
        }

        case 6: {
          vf = vx & 1;
          vx >>= 1;
          break;
        }

        case 7: {
          if (vy > vx) {
            vf = 1;
          } else {
            vf = 0;
          }

          vx = vy - vx;
          if (vx < 0) {
            vx += 0x100;
          }

          break;
        }

        case 0xE: {
          vf = vx & 0x80;
          vx <<= 1;
          if (vx > 0xFF) {
            vx -= 0x100;
          }

          break;
        }
      }

      pc += 2;
    }
  }, {
    0xA000, // ANNN - sets I to the address NNN.
    [](const ChipContext& context) {
      unsigned short& pc = context.registers.getSpecialRegister("PC");
      unsigned short& I = context.registers.getSpecialRegister("I");
      // Store the address segment of the opcode.
      I = context.opcode & 0x0FFF;
      pc += 2;
    }
  }, {
    0xB000, // BNNN - jumps to the address NNN plus V0.
    [](const ChipContext& context) {
      unsigned short& pc = context.registers.getSpecialRegister("PC");
      unsigned char& v0 = context.registers.getDataRegister("V0");
      pc = (context.opcode & 0x0FFF) + v0;
    }
  }, {
    0xC000, // CXNN - sets VX to the result of a bitwise AND operation on a random number and NN.
    [](const ChipContext& context) {
      unsigned short& pc = context.registers.getSpecialRegister("PC");
      std::string registerNameX = "V" + std::to_string((context.opcode & 0x0F00) >> 8);
      unsigned char& vx = context.registers.getDataRegister(registerNameX);
      std::default_random_engine generator;
      std::uniform_int_distribution<unsigned char> distribution(0, 0xFF);
      vx = distribution(generator) & (context.opcode & 0x00FF);
      pc += 2;
    }
  }, {
    /*
     * DXYN - draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height
     * of N pixels. Each row of 8 pixels is read as bit-coded starting from memory location I;
     * I value doesn’t change after the execution of this instruction. As described above, VF
     * is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn,
     * and to 0 if that doesn’t happen.
     */
    0xD000,
    [](const ChipContext& context) {
      unsigned short& pc = context.registers.getSpecialRegister("PC");
      std::string registerNameX = "V" + std::to_string((context.opcode & 0x0F00) >> 8);
      std::string registerNameY = "V" + std::to_string((context.opcode & 0x00F0) >> 4);
      unsigned char& x = context.registers.getDataRegister(registerNameX);
      unsigned char& y = context.registers.getDataRegister(registerNameY);
      unsigned char& vf = context.registers.getDataRegister("V15");
      unsigned short& I = context.registers.getSpecialRegister("I");
      unsigned short height = context.opcode & 0x000F;
      unsigned short pixel;

      vf = 0;
      for (unsigned int yline = 0; yline < height; yline++) {
        pixel = context.memory[I + yline];
        for (unsigned int xline = 0; xline < 8; xline++) {
          if ((pixel & (0x80 >> xline)) != 0) {
            if (buffer[(x + xline + ((y + yline) * 64))] == 0xFFFFFFFF) {
              vf = 1;
            }

            buffer[x + xline + ((y + yline) * 64)] ^= 0xFFFFFFFF;
          }
        }
      }

      drawFlag = true;
      pc += 2;
    }
  }, {
    /*
     * EX9E - skips the next instruction if the key stored in VX is pressed.
     * EXA1 - skips the next instruction if the key stored in VX isn't pressed.
     */
    0xE000,
    [](const ChipContext& context) {
      unsigned short& pc = context.registers.getSpecialRegister("PC");
      std::string registerNameX = "V" + std::to_string((context.opcode & 0x0F00) >> 8);
      unsigned char& vx = context.registers.getDataRegister(registerNameX);
      char keyCode = '\0';
      if (vx >= 0 && vx <= 9) {
        keyCode = vx + 48;
      } else if (vx >= 0xA && vx <= 0xF) {
        keyCode = (vx - 0xA) + 65;
      }

      switch (context.opcode & 0x00FF) {
        case 0x9E: {
          if (keypad.isPressed(keyCode)) {
            pc += 2;
          }
          break;
        }

        case 0xA1: {
          if (!keypad.isPressed(keyCode)) {
            pc += 2;
          }
          break;
        }
      }

      pc += 2;
    }
  }, {
    /*
     * FX07 - sets VX to the value of the delay timer.
     * FX0A - a key press is awaited, and then stored in VX.
     * FX15 - sets the delay timer to VX.
     * FX18 - sets the sound timer to VX.
     * FX1E - adds VX to I.
     * FX29 - sets I to the location of the sprite for the character in VX. Characters 0-F
     *   (in hexadecimal) are represented by a 4x5 font.
     * FX33 - stores the binary-coded decimal representation of VX, with the most significant
     *   of three digits at the address in I, the middle digit at I plus 1, and the least
     *   significant digit at I plus 2. (In other words, take the decimal representation of
     *   VX, place the hundreds digit in memory at location in I, the tens digit at location
     *   I+1, and the ones digit at location I+2.)
     * FX55 - stores V0 to VX (including VX) in memory starting at address I.
     * FX65 - fills V0 to VX (including VX) with values from memory starting at address I.
     */
    0xF000,
    [](const ChipContext& context) {
      unsigned short& pc = context.registers.getSpecialRegister("PC");
      std::string registerNameX = "V" + std::to_string((context.opcode & 0x0F00) >> 8);
      unsigned char& vx = context.registers.getDataRegister(registerNameX);
      unsigned short& dt = context.registers.getSpecialRegister("DT");
      unsigned short& st = context.registers.getSpecialRegister("ST");
      unsigned short& I = context.registers.getSpecialRegister("I");
      int x = (context.opcode & 0x0F00) >> 8;
      switch (context.opcode & 0x00FF) {
        case 0x07: {
          vx = static_cast<unsigned char>(dt);
          break;
        }

        case 0x0A: {
          keyPressFuture = keyPressPromise.get_future();
          // Block until we get a key press.
          keyPressFuture.wait();
          // Store that key press in VX.
          vx = keyPressFuture.get();
          keyPressPromise = std::promise<unsigned char>();
          keyPressFuture = std::future<unsigned char>();
          keySent = false;
          break;
        }

        case 0x15: {
          dt = static_cast<unsigned short>(vx);
          break;
        }

        case 0x18: {
          st = static_cast<unsigned short>(vx);
          break;
        }

        case 0x1E: {
          I += static_cast<unsigned short>(vx);
          break;
        }

        case 0x29: {
          I = fontLocation + 5 * vx;
          break;
        }

        case 0x33: {
          context.memory[I] = vx / 100;
          context.memory[I + 1] = (vx / 10) % 10;
          context.memory[I + 2] = (vx % 100) % 10;
          break;
        }

        case 0x55: {
          for (int i = 0; i <= x; i++) {
            std::string registerNameI = "V" + std::to_string(i);
            unsigned char& vi = context.registers.getDataRegister(registerNameI);
            context.memory[I + i] = vi;
          }
          break;
        }

        case 0x65: {
          for (int i = 0; i <= x; i++) {
            std::string registerNameI = "V" + std::to_string(i);
            unsigned char& vi = context.registers.getDataRegister(registerNameI);
            vi = context.memory[I + i];
          }
          break;
        }
      }

      pc += 2;
    }
  }
});

#endif