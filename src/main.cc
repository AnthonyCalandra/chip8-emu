#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include <thread>
#include <SDL2/SDL.h>
#include "cpu.hh"
#include "sdlutils.hh"

std::array<unsigned char, 80> chip8Fontset{
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cout << "Usage: emu ROMPath" << std::endl;
    return 1;
  }

  if (SDL_Init(SDL_INIT_VIDEO) != 0){
    std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
    return 1;
  }

  std::ifstream input;
  input.open(argv[1], std::ios::binary);
  if (input.fail()) {
    std::cout << "Failed to open file." << std::endl;
    SDL_Quit();
    return 1;
  }

  auto inputIt = std::istreambuf_iterator<char>(input);
  auto eos = std::istreambuf_iterator<char>();
  std::vector<char> romBuffer(inputIt, eos);
  Registers registers;
  MemoryStore memory;
  Stack stack;
  memory.loadRom(std::move(romBuffer));
  memory.loadFont(std::move(chip8Fontset));
  input.close();

  const char* windowTitle = "CHIP-8 Emulator";
  SDL_Window* window = SDL_CreateWindow(windowTitle, 100, 100, screenWidth, screenHeight, SDL_WINDOW_SHOWN);
  if (window == nullptr){
    std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
    SDL_Quit();
    return 1;
  }

  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (renderer == nullptr) {
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  std::thread chipThread(executeChipCpu, std::move(registers), std::move(memory), std::move(stack));
  gameLoop(window, renderer);

  chipThread.join();
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}