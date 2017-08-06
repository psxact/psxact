#include "cpu_cop0.hpp"

void cop0::run(cpu_state_t &state, uint32_t n) {
  switch (n) {
  case 0x10:
    return leave_exception(state);

  default:
    return;
  }
}

uint32_t cop0::read_ccr(cpu_state_t &state, uint32_t n) {
  return 0;
}

void cop0::write_ccr(cpu_state_t &state, uint32_t n, uint32_t value) {
}

uint32_t cop0::read_gpr(cpu_state_t &state, uint32_t n) {
  return state.cop0.regs[n];
}

void cop0::write_gpr(cpu_state_t &state, uint32_t n, uint32_t value) {
  state.cop0.regs[n] = value;
}

void cop0::enter_exception(cpu_state_t &state, uint32_t code) {
  uint32_t status = state.cop0.regs[12];
  status = (status & ~0x3f) | ((status << 2) & 0x3f);

  uint32_t cause = state.cop0.regs[13];
  cause = (cause & ~0x7f) | ((code << 2) & 0x7f);

  uint32_t epc;

  if (state.is_branch_delay_slot) {
    epc = state.regs.this_pc - 4;
    cause |= 0x80000000;
  }
  else {
    epc = state.regs.this_pc;
    cause &= ~0x80000000;
  }

  state.cop0.regs[12] = status;
  state.cop0.regs[13] = cause;
  state.cop0.regs[14] = epc;

  state.regs.pc = (status & (1 << 22))
                   ? 0xbfc00180
                   : 0x80000080;

  state.regs.next_pc = state.regs.pc + 4;
}

void cop0::leave_exception(cpu_state_t &state) {
  uint32_t sr = state.cop0.regs[12];
  sr = (sr & ~0xf) | ((sr >> 2) & 0xf);

  state.cop0.regs[12] = sr;
}
