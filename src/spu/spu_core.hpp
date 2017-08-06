#ifndef __PSXACT_SPU_CORE_HPP__
#define __PSXACT_SPU_CORE_HPP__

#include "../bus.hpp"
#include "../state.hpp"

namespace spu {
  uint32_t io_read(spu_state_t &state, bus_width_t width, uint32_t address);

  void io_write(spu_state_t &state, bus_width_t width, uint32_t address, uint32_t data);
}

#endif // __PSXACT_SPU_CORE_HPP__
