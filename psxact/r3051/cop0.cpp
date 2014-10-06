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

void Cop0::init(void) {
  memset(this, 0, sizeof(Cop0));

  this->registers[15] = 0x00000002; // imp=0, rev=2
}

void Cop0::kill(void) {
}

uint32_t Cop0::fetch_sr(uint32_t reg) {
  return this->registers[reg] & registers_mask[reg];
}

void Cop0::store_sr(uint32_t reg, uint32_t data) {
  this->registers[reg] = data & registers_mask[reg];
}

uint32_t Cop0::fetch_cr(uint32_t reg) {
  return 0;
}

void Cop0::store_cr(uint32_t reg, uint32_t data) {
}
