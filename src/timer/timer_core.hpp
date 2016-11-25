#ifndef PSXACT_TIMER_CORE_HPP
#define PSXACT_TIMER_CORE_HPP

#include <cstdint>

namespace timer {
  uint32_t bus_read(int width, uint32_t address);

  void bus_write(int width, uint32_t address, uint32_t data);
}

#endif //PSXACT_TIMER_CORE_HPP
