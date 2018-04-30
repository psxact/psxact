#include "cpu/cpu-cop0.hpp"


void cpu_cop0_t::run(uint32_t n) {
  if (n == 0x10) {
    return rfe();
  }
}


uint32_t cpu_cop0_t::read_ccr(uint32_t n) {
  return 0;
}


void cpu_cop0_t::write_ccr(uint32_t n, uint32_t value) {
}


uint32_t cpu_cop0_t::read_gpr(uint32_t n) {
  return regs[n];
}


void cpu_cop0_t::write_gpr(uint32_t n, uint32_t value) {
  regs[n] = value;
}


void cpu_cop0_t::rfe() {
  uint32_t sr = read_gpr(12);
  sr = (sr & ~0xf) | ((sr >> 2) & 0xf);

  write_gpr(12, sr);
}
