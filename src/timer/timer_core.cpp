#include "timer_core.hpp"
#include "../bus.hpp"
#include "../utility.hpp"
#include "../state.hpp"

uint32_t timer::io_read(timer_state_t *state, int width, uint32_t address) {
  if (utility::log_timer) {
    printf("timer::io_read(%d, 0x%08x)\n", width, address);
  }

  int n = (address >> 4) & 3;

  switch ((address & 0xf) / 4) {
  case 0: return state->timers[n].counter;
  case 1: return state->timers[n].control;
  case 2: return state->timers[n].compare;
  }

  return 0;
}

void timer::io_write(timer_state_t *state, int width, uint32_t address, uint32_t data) {
  if (utility::log_timer) {
    printf("timer::io_write(%d, 0x%08x, 0x%08x)\n", width, address, data);
  }

  int n = (address >> 4) & 3;

  switch ((address & 0xf) / 4) {
  case 0:
    state->timers[n].counter = uint16_t(data);
    break;

  case 1:
    state->timers[n].control = uint16_t(data | 0x400);
    state->timers[n].counter = 0;
    break;

  case 2:
    state->timers[n].compare = uint16_t(data);
    break;
  }
}

void timer::tick_timer_0(timer_state_t *state) {
}

void timer::tick_timer_1(timer_state_t *state) {
}

void timer::tick_timer_2(timer_state_t *state) {
  // system clock/8
  state->timers[2].divider++;

  if (state->timers[2].divider == 8) {
    state->timers[2].divider = 0;
    state->timers[2].counter++;

    if (state->timers[2].counter == state->timers[2].compare) {
      state->timers[2].control |= 0x800;

      if (state->timers[2].control & 0x0008) {
        state->timers[2].counter = 0;
      }

      if (state->timers[2].control & 0x0010) {
        state->timers[2].control &= ~0x0400;
        bus::irq(6);
      }
    }
  }
}
