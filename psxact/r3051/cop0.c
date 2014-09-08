#include "stdafx.h"
#include "cop0.h"

static uint32_t registers_mask[16] = {
  0xffffffff, // r0 - locked
  0xffffffff, // r1 - locked
  0xffffffff, // r2 - locked
  0xffffffff, // r3 - ?
  0xffffffff, // r4 - locked
  0xffffffff, // r5 - locked
  0xffffffff, // r6 - ?
  0xffffffff, // r7 - locked
  0xffffffff, // r8 - badvaddr
  0xffffffff, // r9 - locked
  0xffffffff, // r10 - locked
  0xffffffff, // r11 - locked
  0xffffffff, // r12 - sr
  0xffffffff, // r13 - cause
  0xffffffff, // r14 - epc
  0xffffffff, // r15 - prid
};

void r3051_cop0_init(struct r3051_cop0* cop) {
  memset(cop, 0, sizeof(struct r3051_cop0));

  cop->registers[15] = 0x00000302; // imp=3, rev=2
}

void r3051_cop0_kill(struct r3051_cop0* cop) {
}

uint32_t r3051_cop0_fetch_sr(struct r3051_cop0* cop, uint32_t reg) {
  return cop->registers[reg] & registers_mask[reg];
}

void r3051_cop0_store_sr(struct r3051_cop0* cop, uint32_t reg, uint32_t data) {
  cop->registers[reg] = data & registers_mask[reg];
}

uint32_t r3051_cop0_fetch_cr(struct r3051_cop0* cop, uint32_t reg) {
  return 0;
}

void r3051_cop0_store_cr(struct r3051_cop0* cop, uint32_t reg, uint32_t data) {
}
