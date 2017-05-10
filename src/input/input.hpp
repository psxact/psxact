#ifndef __PSXACT_INPUT_HPP__
#define __PSXACT_INPUT_HPP__

#include <cstdint>
#include "../state.hpp"

namespace input {
  uint32_t io_read(input_state_t *state, int width, uint32_t address);

  void io_write(input_state_t *state, int width, uint32_t address, uint32_t data);
}

#endif // __PSXACT_INPUT_HPP__
