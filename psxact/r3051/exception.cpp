#include "stdafx.h"
#include "r3051.h"
#include "cop0.h"

void Cop0::syscall(R3051* processor) {
  uint32_t cause = this->registers[13];

  // ExCode = 8

  cause &= ~0x7c;
  cause |= 0x08 << 2;

  this->registers[13] = cause;
  this->registers[14] = processor->ex.address;

  processor->pc = 0x80000080;
  processor->rf.code = 0;
}
