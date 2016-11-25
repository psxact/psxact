#include "vram.hpp"
#include "../utility.hpp"

static utility::memory_t<20> buffer;

uint16_t* vram::get_pointer() {
  return &buffer.h[0];
}

uint16_t vram::read(int x, int y) {
  return buffer.h[(y * 1024) + x];
}

void vram::write(int x, int y, uint16_t data) {
  buffer.h[(y * 1024) + x] = data;
}
