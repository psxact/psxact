#include "stdafx.h"
#include "r3051.h"

struct r3051* processor;

void r3051_init(struct r3051* processor) {
  for (int i = 0; i < 32; i++) {
    processor->registers[i] = 0x00000000;
  }

  processor->registers[31] = 0xff;
}

void r3051_step(struct r3051* processor) {
  r3051_wb(processor);
  r3051_dc(processor);
  r3051_ex(processor);
  r3051_rf(processor);
  r3051_ic(processor);
}
