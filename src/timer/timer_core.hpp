#ifndef __PSXACT_TIMER_CORE_HPP__
#define __PSXACT_TIMER_CORE_HPP__

#include "../state.hpp"

namespace timer {
  uint32_t io_read(timer_state_t *state, int width, uint32_t address);

  void io_write(timer_state_t *state, int width, uint32_t address, uint32_t data);

  void tick_timer_0(timer_state_t *state);

  void tick_timer_1(timer_state_t *state);

  void tick_timer_2(timer_state_t *state);
}

#endif // __PSXACT_TIMER_CORE_HPP__
