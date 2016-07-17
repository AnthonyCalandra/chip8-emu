#ifndef SDLUTILS_HH

#define SDLUTILS_HH 1
#include <SDL2/SDL.h>
#include <string>
#include <array>
#include "videobuffer.hh"
#include "keypad.hh"

extern Keypad keypad;
extern VideoBuffer buffer;
extern std::atomic_bool drawFlag;

void gameLoop(SDL_Window* window, SDL_Renderer* renderer);
bool renderTexture(SDL_Renderer* renderer, VideoBuffer& buffer, unsigned int x, unsigned int y);

inline bool isChip8KeyEvent(SDL_Keycode keyCode) {
  return (keyCode >= SDLK_0 && keyCode <= SDLK_9) || (keyCode >= SDLK_a && keyCode <= SDLK_f);
}

#endif