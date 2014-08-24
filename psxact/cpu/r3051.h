#include "stdafx.h"

struct r3051_stage {
  uint32_t address;
  uint32_t code;
};

struct r3051 {
  uint32_t registers[32];

  struct r3051_stage ic;
  struct r3051_stage rf;
  struct r3051_stage ex;
  struct r3051_stage dc;
  struct r3051_stage wb;
};

void psx_cpu_step(struct r3051*);
void psx_cpu_init(struct r3051*);

#define STAGE(name) void psx_cpu_##name(struct r3051* processor)

STAGE(ic);
STAGE(rf);
STAGE(ex);
STAGE(dc);
STAGE(wb);
