#ifndef PSXACT_GPU_CORE_HPP
#define PSXACT_GPU_CORE_HPP

#include <cstdint>

namespace gpu {
  struct state_t {
    uint32_t status;
    bool textured_rectangle_x_flip;
    bool textured_rectangle_y_flip;
  };

  uint32_t read(int size, uint32_t address);

  void write(int size, uint32_t address, uint32_t data);
}

#endif //PSXACT_GPU_CORE_HPP
