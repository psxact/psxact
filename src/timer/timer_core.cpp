#include "timer_core.hpp"
#include "../utility.hpp"

uint32_t timer::io_read(timer_state_t &state, bus::bus_width_t width, uint32_t address) {
  if (utility::log_timer) {
    printf("timer::io_read(%d, 0x%08x)\n", width, address);
  }

  int n = (address >> 4) & 3;

  switch ((address & 0xf) / 4) {
  case 0: return state.timers[n].counter;
  case 1: return state.timers[n].control;
  case 2: return state.timers[n].compare;
  }

  return 0;
}

void timer::io_write(timer_state_t &state, bus::bus_width_t width, uint32_t address, uint32_t data) {
  if (utility::log_timer) {
    printf("timer::io_write(%d, 0x%08x, 0x%08x)\n", width, address, data);
  }

  int n = (address >> 4) & 3;

  switch ((address & 0xf) / 4) {
  case 0:
    state.timers[n].counter = uint16_t(data);
    break;

  case 1:
    state.timers[n].control = uint16_t(data | 0x400);
    state.timers[n].counter = 0;
    break;

  case 2:
    state.timers[n].compare = uint16_t(data);
    break;
  }
}

static void tick_timer_0(timer_state_t &state) {
}

static void tick_timer_1(timer_state_t &state) {
  auto &timer = state.timers[1];

  timer.divider += 11;

  if (timer.divider >= 3413 * 7) {
    timer.divider -= 3413 * 7;
    timer.counter += 1;
  }
}

static void tick_timer_2(timer_state_t &state) {
  auto &timer = state.timers[2];

  // system clock/8
  timer.divider++;

  if (timer.divider == 8) {
    timer.divider = 0;
    timer.counter++;

    if (timer.counter == timer.compare) {
      timer.control |= 0x800;

      if (timer.control & 0x0008) {
        timer.counter = 0;
      }

      if (timer.control & 0x0010) {
        timer.control &= ~0x0400;
        bus::irq(6);
      }
    }
  }
}

void timer::tick(timer_state_t &state) {
  tick_timer_0(state);
  tick_timer_1(state);
  tick_timer_2(state);
}
