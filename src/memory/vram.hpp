#ifndef __PSXACT_VRAM_HPP__
#define __PSXACT_VRAM_HPP__

#include <cstdint>

namespace vram {
  uint16_t *get_pointer();

  uint16_t read(int32_t x, int32_t y);

  void write(int32_t x, int32_t y, uint16_t data);
}

#endif // __PSXACT_VRAM_HPP__
