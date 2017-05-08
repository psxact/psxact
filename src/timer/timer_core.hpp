#ifndef __PSXACT_TIMER_CORE_HPP__
#define __PSXACT_TIMER_CORE_HPP__

#include <cstdint>

namespace timer {
  struct state_t {
    uint16_t counter;
    uint16_t control;
    uint16_t compare;
    int divider;
  };

  uint32_t bus_read(int width, uint32_t address);

  void bus_write(int width, uint32_t address, uint32_t data);

  void tick_timer_0();

  void tick_timer_1();

  void tick_timer_2();
}

#endif // __PSXACT_TIMER_CORE_HPP__
