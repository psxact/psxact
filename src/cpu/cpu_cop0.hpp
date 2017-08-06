#ifndef __PSXACT_CPU_COP0_HPP__
#define __PSXACT_CPU_COP0_HPP__

#include "../state.hpp"

namespace cop0 {
  void run(cpu_state_t &state, uint32_t n);

  uint32_t read_ccr(cpu_state_t &state, uint32_t n);

  void write_ccr(cpu_state_t &state, uint32_t n, uint32_t value);

  uint32_t read_gpr(cpu_state_t &state, uint32_t n);

  void write_gpr(cpu_state_t &state, uint32_t n, uint32_t value);

  void enter_exception(cpu_state_t &state, uint32_t code);

  void leave_exception(cpu_state_t &state);
}

#endif //__PSXACT_CPU_COP0_HPP__
