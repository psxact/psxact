#include "stdafx.h"
#include "cp0.h"

extern struct r3051_cp0* cp0;

static uint32_t registers_mask[16] = {
  0x00000000, // r0 - locked
  0x00000000, // r1 - locked
  0x00000000, // r2 - locked
  0xffffffff, // r3 - ?
  0x00000000, // r4 - locked
  0x00000000, // r5 - locked
  0xffffffff, // r6 - ?
  0x00000000, // r7 - locked
  0xffffffff, // r8 - badvaddr
  0x00000000, // r9 - locked
  0x00000000, // r10 - locked
  0x00000000, // r11 - locked
  0xf04fff3f, // r12 - sr
  0xb000ff7c, // r13 - cause
  0xffffffff, // r14 - epc
  0x0000ffff, // r15 - prid
};

void r3051_cp0_init(struct r3051_cp0* cp) {
  cp->registers[15] = 0x00000302; // imp=3, rev=2
}

void r3051_cp0_fetch(struct r3051_cp0* cp, uint32_t reg, uint32_t* data) {
  *data = cp0->registers[reg] & registers_mask[reg];
}

void r3051_cp0_store(struct r3051_cp0* cp, uint32_t reg, uint32_t* data) {
  cp0->registers[reg] = *data & registers_mask[reg];
}
