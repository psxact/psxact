#include "stdafx.hpp"
#include "cop0.hpp"
#include "r3051.hpp"

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

Cop0::Cop0(void)
  : registers() {
  this->registers[15] = 0x00000002; // imp=0, rev=2
}

Cop0::~Cop0(void) {
}

uint32_t Cop0::FetchSr(uint32_t index) {
  return this->registers[index] & registers_mask[index];
}

void Cop0::StoreSr(uint32_t index, uint32_t data) {
  this->registers[index] = data;
}

uint32_t Cop0::FetchCr(uint32_t index) {
  return 0;
}

void Cop0::StoreCr(uint32_t index, uint32_t data) {
}

bool Cop0::Execute(uint32_t code) {
  switch (code) {
  case 0x10: // RFE
    this->registers[12] &= ~0x0f;
    this->registers[12] |= (this->registers[12] >> 2) & 0x0f;
    return true;
  }

  return false;
}

void Cop0::SysCall(R3051* processor) {
  uint32_t cause = this->registers[13];

  // ExCode = 8

  cause &= ~0x7c;
  cause |= 0x08 << 2;

  this->registers[13] = cause;
  this->registers[14] = processor->ex.address;

  processor->pc = 0x80000080;
  processor->rf.code = 0;
}
