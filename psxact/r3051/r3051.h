#include "stdafx.h"

static uint32_t register_masks[32];

struct r3051_pipestage {
  uint32_t address;
  uint32_t code;
};

struct r3051 {
  uint32_t registers[32];
  uint32_t lo;
  uint32_t hi;
  uint32_t pc;

  struct r3051_pipestage ic;
  struct r3051_pipestage rf;
  struct r3051_pipestage ex;
  struct r3051_pipestage dc;
  struct r3051_pipestage wb;
};

void r3051_bus_read(int, uint32_t, uint32_t*);
void r3051_bus_write(int, uint32_t, uint32_t);

void r3051_step(struct r3051*);
void r3051_init(struct r3051*);

#define STAGE(name) void r3051_stage_##name(struct r3051* processor)

STAGE(ic);
STAGE(rf);
STAGE(ex);
STAGE(dc);
STAGE(wb);
