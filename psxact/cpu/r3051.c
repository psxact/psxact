#include "stdafx.h"
#include "r3051.h"

struct r3051* processor;

void psx_cpu_init(struct r3051* processor) {
  for (int i = 0; i < 32; i++) {
    processor->registers[i] = 0x00000000;
  }

  processor->registers[31] = 0xff;
}

void psx_cpu_step(struct r3051* processor) {
  psx_cpu_wb(processor);
  psx_cpu_dc(processor);
  psx_cpu_ex(processor);
  psx_cpu_rf(processor);
  psx_cpu_ic(processor);
}
