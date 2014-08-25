#include "stdafx.h"
#include "r3051.h"

uint32_t register_masks[32] = {
  0x00000000, 0xffffffff, 0xffffffff, 0xffffffff,
  0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
  0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
  0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
  0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
  0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
  0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
  0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff
};

struct r3051* processor;

void r3051_init(struct r3051* processor) {
  for (int i = 0; i < 32; i++) {
    processor->registers[i] = 0x00000000;
  }

  processor->pc = 0xbfc00000;
  processor->lo = 0x00000000;
  processor->hi = 0x00000000;
}

void r3051_step(struct r3051* processor) {
  r3051_stage_wb(processor);
  r3051_stage_dc(processor);
  r3051_stage_ex(processor);
  r3051_stage_rf(processor);
  r3051_stage_ic(processor);
}
