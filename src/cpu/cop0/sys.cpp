// Copyright 2018 psxact

#include "cpu/cop0/sys.hpp"

using namespace psx::cpu::cop0;

void sys_t::run(uint32_t n) {
  if (n == 0x10) {
    return rfe();
  }
}

uint32_t sys_t::read_ccr(uint32_t) {
  return 0;
}

void sys_t::write_ccr(uint32_t, uint32_t) {
}

uint32_t sys_t::read_gpr(uint32_t n) {
  return regs[n];
}

void sys_t::write_gpr(uint32_t n, uint32_t value) {
  regs[n] = value;
}

void sys_t::rfe() {
  uint32_t sr = read_gpr(12);
  sr = (sr & ~0xf) | ((sr >> 2) & 0xf);

  write_gpr(12, sr);
}
