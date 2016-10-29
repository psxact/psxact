#ifndef PSXACT_TIMER_CORE_HPP
#define PSXACT_TIMER_CORE_HPP

#include <cstdint>

namespace timer {
  struct state_t {
    uint32_t counter;
    uint32_t control;
    uint32_t target;
  };

  uint32_t mmio_read(int size, uint32_t address);

  void mmio_write(int size, uint32_t address, uint32_t data);
}

#endif //PSXACT_TIMER_CORE_HPP
