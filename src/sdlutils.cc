#include <SDL2/SDL.h>
#include <atomic>
#include <array>
#include <future>
#include <iostream>
#include "sdlutils.hh"
#include "videobuffer.hh"
#include "keypad.hh"

Keypad keypad{};
VideoBuffer buffer{};
std::atomic_bool drawFlag{false};
std::atomic_bool quit{false};
std::atomic_bool keySent{false};
std::promise<unsigned char> keyPressPromise;
std::future<unsigned char> keyPressFuture;

void gameLoop(SDL_Window* window, SDL_Renderer* renderer) {
  SDL_Event event;
  buffer.clear();
  while (!quit) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        quit = true;
      }

      if (event.type == SDL_KEYDOWN) {
        SDL_Keycode keyCode = event.key.keysym.sym;
        if (isChip8KeyEvent(keyCode)) {
          const char* keyName = SDL_GetKeyName(keyCode);
          keypad.pressedDown(keyName[0]);
          if (!keySent) {
            keyPressPromise.set_value(keyName[0]);
            keySent = true;
          }
        }
      } else if (event.type == SDL_KEYUP) {
        SDL_Keycode keyCode = event.key.keysym.sym;
        if (isChip8KeyEvent(keyCode)) {
          const char* keyName = SDL_GetKeyName(keyCode);
          keypad.pressedUp(keyName[0]);
        }
      }
    }

    if (drawFlag) {
      renderTexture(renderer, buffer, 0, 0);
      SDL_RenderPresent(renderer);
      drawFlag = false;
    }

    SDL_Delay(5);
  }
}

bool renderTexture(SDL_Renderer* renderer, VideoBuffer& buffer, unsigned int x, unsigned int y) {
  std::size_t depth = sizeof(VideoBufferPixelType) * 8;
  std::size_t pitch = chip8ScreenWidth * sizeof(VideoBufferPixelType);
  SDL_Surface* surface = SDL_CreateRGBSurfaceFrom((void*) buffer.getBuffer().data(), chip8ScreenWidth, chip8ScreenHeight, depth, pitch, 0, 0, 0, 0);
  SDL_Texture* texture = nullptr;
  if (surface != nullptr) {
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (surface == nullptr) {
      return false;
    }
  } else {
    return false;
  }

  SDL_Rect dst;
  dst.x = x;
  dst.y = y;
  SDL_QueryTexture(texture, NULL, NULL, &dst.w, &dst.h);
  dst.h *= 10;
  dst.w *= 10;
  SDL_RenderCopy(renderer, texture, nullptr, &dst);
  return true;
}