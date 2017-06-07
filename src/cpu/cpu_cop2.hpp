#ifndef __PSXACT_CPU_COP2_HPP__
#define __PSXACT_CPU_COP2_HPP__

#include "../state.hpp"

namespace cop2 {
  void run(cop2_state_t *state, uint32_t n);

  uint32_t read_ccr(cop2_state_t *state, uint32_t n);
  void write_ccr(cop2_state_t *state, uint32_t n, uint32_t value);

  uint32_t read_gpr(cop2_state_t *state, uint32_t n);
  void write_gpr(cop2_state_t *state, uint32_t n, uint32_t value);
}

#endif //__PSXACT_CPU_COP2_HPP__
