#include "cpu_cop0.hpp"

namespace psxact {
  void cop0_core::run(uint32_t n) {
    switch (n) {
    case 0x10:
      return leave_exception();

    default:
      return;
    }
  }

  uint32_t cop0_core::read_ccr(uint32_t n) {
    return 0;
  }

  void cop0_core::write_ccr(uint32_t n, uint32_t value) {
  }

  uint32_t cop0_core::read_gpr(uint32_t n) {
    return regs[n];
  }

  void cop0_core::write_gpr(uint32_t n, uint32_t value) {
    regs[n] = value;
  }

  uint32_t cop0_core::enter_exception(uint32_t code, uint32_t pc, bool is_branch_delay_slot) {
    uint32_t status = regs[12];
    status = (status & ~0x3f) | ((status << 2) & 0x3f);

    uint32_t cause = regs[13];
    cause = (cause & ~0x7f) | ((code << 2) & 0x7f);

    uint32_t epc;

    if (is_branch_delay_slot) {
      epc = pc - 4;
      cause |= 0x80000000;
    }
    else {
      epc = pc;
      cause &= ~0x80000000;
    }

    regs[12] = status;
    regs[13] = cause;
    regs[14] = epc;

    return (status & (1 << 22))
           ? 0xbfc00180
           : 0x80000080;
  }

  void cop0_core::leave_exception() {
    uint32_t sr = regs[12];
    sr = (sr & ~0xf) | ((sr >> 2) & 0xf);

    regs[12] = sr;
  }
}
