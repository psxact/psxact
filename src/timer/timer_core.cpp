#include "timer_core.hpp"
#include "../utility.hpp"

using namespace psxact;
using namespace psxact::timer;

uint32_t core::io_read(bus_width_t width, uint32_t address) {
  if (utility::log_timer) {
    printf("timer::io_read(%d, 0x%08x)\n", width, address);
  }

  int n = (address >> 4) & 3;

  switch ((address & 0xf) / 4) {
  case 0:
    return timers[n].counter;

  case 1:
    return timers[n].control;

  case 2:
    return timers[n].compare;
  }

  return 0;
}

void core::io_write(bus_width_t width, uint32_t address, uint32_t data) {
  if (utility::log_timer) {
    printf("timer::io_write(%d, 0x%08x, 0x%08x)\n", width, address, data);
  }

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

void core::tick() {
  tick_timer_0();
  tick_timer_1();
  tick_timer_2();
}

void core::tick_timer_0() {
}

void core::tick_timer_1() {
  auto &timer = timers[1];

  timer.divider += 11;

  if (timer.divider >= 3413 * 7) {
    timer.divider -= 3413 * 7;
    timer.counter += 1;
  }
}

void core::tick_timer_2() {
  auto &timer = timers[2];

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
        system->irq(6);
      }
    }
  }
}
