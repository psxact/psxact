#include "vram.hpp"
#include "../utility.hpp"

namespace psxact {
  static memory_t<20> buffer;

  uint16_t *vram::get_pointer() {
    return &buffer.h[0];
  }

  uint16_t vram::read(int32_t x, int32_t y) {
    return buffer.h[(y * 1024) + x];
  }

  void vram::write(int32_t x, int32_t y, uint16_t data) {
    if (x < 0 || x > 0x400) return;
    if (y < 0 || y > 0x200) return;

    buffer.h[(y * 1024) + x] = data;
  }
}
