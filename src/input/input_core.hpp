#ifndef __PSXACT_INPUT_HPP__
#define __PSXACT_INPUT_HPP__

#include "../bus.hpp"
#include "../state.hpp"

namespace input {
  uint32_t io_read(input_state_t &state, bus::bus_width_t width, uint32_t address);

  void io_write(input_state_t &state, bus::bus_width_t width, uint32_t address, uint32_t data);

  void init(input_state_t &state);

  void tick(input_state_t &state);
}

#endif // __PSXACT_INPUT_HPP__
