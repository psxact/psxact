#ifndef __PSXACT_SPU_CORE_HPP__
#define __PSXACT_SPU_CORE_HPP__

#include <stdint.h>

namespace spu {
  uint32_t io_read(int width, uint32_t address);

  void io_write(int width, uint32_t address, uint32_t data);
}

#endif // __PSXACT_SPU_CORE_HPP__
