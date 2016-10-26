#ifndef __GPU_CORE_HPP__
#define __GPU_CORE_HPP__

#include <stdint.h>

namespace gpu {
  uint32_t read(uint32_t);

  void write(uint32_t, uint32_t);
}

#endif
