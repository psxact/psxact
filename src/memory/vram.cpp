#include "memory/vram.hpp"
#include "utility.hpp"


static memory_t<20> buffer;


#define value_at(x, y) \
  buffer.h[((x) + ((y) * 1024))]


uint16_t *vram::get_pointer(int32_t x, int32_t y) {
  return &value_at(x, y);
}


uint16_t vram::read(int32_t x, int32_t y) {
  return value_at(x, y);
}


void vram::write(int32_t x, int32_t y, uint16_t data) {
  if (x & ~0x3ff) return;
  if (y & ~0x1ff) return;

  value_at(x, y) = data;
}


#undef value_at
