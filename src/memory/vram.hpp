#ifndef __psxact_vram_hpp__
#define __psxact_vram_hpp__


#include <cstdint>


namespace vram {
  uint16_t *get_pointer(int32_t x, int32_t y);

  uint16_t read(int32_t x, int32_t y);

  void write(int32_t x, int32_t y, uint16_t data);
}


#endif // __psxact_vram_hpp__
