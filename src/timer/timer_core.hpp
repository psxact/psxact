#ifndef __PSXACT_TIMER_CORE_HPP__
#define __PSXACT_TIMER_CORE_HPP__

#include "../bus.hpp"
#include "../state.hpp"

namespace timer {
  void tick(timer_state_t &state);

  uint32_t io_read(timer_state_t &state, bus_width_t width, uint32_t address);

  void io_write(timer_state_t &state, bus_width_t width, uint32_t address, uint32_t data);
}

#endif // __PSXACT_TIMER_CORE_HPP__
