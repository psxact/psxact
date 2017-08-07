#ifndef __PSXACT_CPU_COP0_HPP__
#define __PSXACT_CPU_COP0_HPP__

#include <cstdint>

struct cop0_core {
  uint32_t regs[16];

  void run(uint32_t n);

  uint32_t read_ccr(uint32_t n);

  void write_ccr(uint32_t n, uint32_t value);

  uint32_t read_gpr(uint32_t n);

  void write_gpr(uint32_t n, uint32_t value);

  uint32_t enter_exception(uint32_t code, uint32_t pc, bool is_branch_delay_slot);

  void leave_exception();
};

#endif //__PSXACT_CPU_COP0_HPP__
