#ifndef VIDEOBUFFER_HH

#define VIDEOBUFFER_HH 1
#include <thread>
#include <array>

constexpr std::size_t chip8ScreenWidth = 64;
constexpr std::size_t chip8ScreenHeight = 32;
constexpr std::size_t screenWidth = chip8ScreenWidth * 10;
constexpr std::size_t screenHeight = chip8ScreenHeight * 10;

using VideoBufferPixelType = unsigned int;
using VideoBufferType = std::array<VideoBufferPixelType, chip8ScreenWidth * chip8ScreenHeight>;
const VideoBufferPixelType fillColor = 0;

class VideoBuffer {
  VideoBufferType buffer{};
  std::mutex mutex;

public:
  void clear();
  VideoBufferType getBuffer();
  VideoBufferPixelType& operator[](std::size_t index);
};

#endif