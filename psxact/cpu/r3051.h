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

void r3051_step(struct r3051*);
void r3051_init(struct r3051*);

#define STAGE(name) void r3051_##name(struct r3051* processor)

STAGE(ic);
STAGE(rf);
STAGE(ex);
STAGE(dc);
STAGE(wb);
