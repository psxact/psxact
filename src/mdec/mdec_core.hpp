#ifndef __PSXACT_MDEC_CORE_HPP__
#define __PSXACT_MDEC_CORE_HPP__

#include "../bus.hpp"

struct mdec_core {
  uint32_t io_read(bus_width_t width, uint32_t address);

  void io_write(bus_width_t width, uint32_t address, uint32_t data);
};

#endif // __PSXACT_MDEC_CORE_HPP__
