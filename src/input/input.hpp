#ifndef __PSXACT_INPUT_HPP__
#define __PSXACT_INPUT_HPP__

#include <cstdint>

namespace input {
  struct state_t {
    uint32_t status;
  };

  uint32_t io_read(int width, uint32_t address);

  void io_write(int width, uint32_t address, uint32_t data);
}

#endif // __PSXACT_INPUT_HPP__
