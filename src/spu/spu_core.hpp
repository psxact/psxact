#ifndef __SPU_CORE_HPP__
#define __SPU_CORE_HPP__

#include <stdint.h>

namespace spu {
  uint32_t bus_read(int width, uint32_t address);

  void bus_write(int width, uint32_t address, uint32_t data);
}

#endif
