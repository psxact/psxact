#ifndef __PSXACT_TIMER_CORE_HPP__
#define __PSXACT_TIMER_CORE_HPP__

#include "../system_core.hpp"

namespace psxact {
namespace timer {

  struct core {
    struct {
      uint16_t counter;
      uint16_t control;
      uint16_t compare;
      int32_t divider;
    } timers[3];

    uint32_t io_read(bus_width_t width, uint32_t address);

    void io_write(bus_width_t width, uint32_t address, uint32_t data);

    void tick();

    void tick_timer_0();

    void tick_timer_1();

    void tick_timer_2();
  };

}
}

#endif // __PSXACT_TIMER_CORE_HPP__
