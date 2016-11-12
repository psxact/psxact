#ifndef __SPU_CORE_HPP__
#define __SPU_CORE_HPP__

#include <stdint.h>

namespace spu {
  uint32_t mmio_read(int size, uint32_t address);

  void mmio_write(int size, uint32_t address, uint32_t data);
}

#endif
