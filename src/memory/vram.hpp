#ifndef PSXACT_VRAM_HPP
#define PSXACT_VRAM_HPP

#include <cstdint>

namespace vram {
  uint16_t *get_pointer();

  uint16_t read(int x, int y);

  void write(int x, int y, uint16_t data);
}

#endif //PSXACT_VRAM_HPP
