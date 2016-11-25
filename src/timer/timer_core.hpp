#ifndef PSXACT_TIMER_CORE_HPP
#define PSXACT_TIMER_CORE_HPP

#include <cstdint>

namespace timer {
  uint32_t bus_read(int size, uint32_t address);

  void bus_write(int size, uint32_t address, uint32_t data);
}

#endif //PSXACT_TIMER_CORE_HPP
