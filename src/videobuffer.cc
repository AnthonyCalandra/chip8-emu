#include <algorithm>
#include <array>
#include "videobuffer.hh"

void VideoBuffer::clear() {
  this->mutex.lock();
  // Fill with black pixels.
  this->buffer.fill(fillColor);
  this->mutex.unlock();
}

VideoBufferType VideoBuffer::getBuffer() {
  this->mutex.lock();
  VideoBufferType copyBuffer = this->buffer;
  this->mutex.unlock();
  return copyBuffer;
}

VideoBufferPixelType& VideoBuffer::operator[](std::size_t index) {
  return this->buffer[index];
}