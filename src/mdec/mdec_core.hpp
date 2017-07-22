#ifndef __PSXACT_MDEC_CORE_HPP__
#define __PSXACT_MDEC_CORE_HPP__

#include "../bus.hpp"
#include "../state.hpp"

namespace mdec {
  uint32_t io_read(mdec_state_t *state, bus::bus_width_t width, uint32_t address);
  void io_write(mdec_state_t *state, bus::bus_width_t width, uint32_t address, uint32_t data);
};

#endif // __PSXACT_MDEC_CORE_HPP__
