#ifndef __PSXACT_BUS_HPP__
#define __PSXACT_BUS_HPP__

#include <stdint.h>
#include <string>
#include "state.hpp"

enum bus_width_t {
  BUS_WIDTH_BYTE,
  BUS_WIDTH_HALF,
  BUS_WIDTH_WORD
};

namespace bus {
  bool init(system_state_t *state, const char *bios_file_name);

  void irq(int32_t interrupt);

  uint32_t read(bus_width_t width, uint32_t address);

  void write(bus_width_t width, uint32_t address, uint32_t data);
}

#endif // __PSXACT_BUS_HPP__
