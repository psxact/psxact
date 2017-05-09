#include "timer_core.hpp"
#include "../bus.hpp"

static timer::state_t timers[3];

uint32_t timer::io_read(int width, uint32_t address) {
  int n = (address >> 4) & 3;

  switch ((address & 0xf) / 4) {
  case 0: return timers[n].counter;
  case 1: return timers[n].control;
  case 2: return timers[n].compare;
  }

  return 0;
}

void timer::io_write(int width, uint32_t address, uint32_t data) {
  int n = (address >> 4) & 3;

  switch ((address & 0xf) / 4) {
  case 0:
    timers[n].counter = uint16_t(data);
    break;

  case 1:
    timers[n].control = uint16_t(data | 0x400);
    timers[n].counter = 0;
    break;

  case 2:
    timers[n].compare = uint16_t(data);
    break;
  }
}

void timer::tick_timer_0() {
}

void timer::tick_timer_1() {
}

void timer::tick_timer_2() {
  // system clock/8
  timers[2].divider++;

  if (timers[2].divider == 8) {
    timers[2].divider = 0;
    timers[2].counter++;

    if (timers[2].counter == timers[2].compare) {
      timers[2].control |= 0x800;

      if (timers[2].control & 0x0008) {
        timers[2].counter = 0;
      }

      if (timers[2].control & 0x0010) {
        timers[2].control &= ~0x0400;
        bus::irq(6);
      }
    }
  }
}
