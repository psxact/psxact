#include "stdafx.h"
#include "r3051.h"

void r3051_stage_ic(struct r3051* processor) {
  struct r3051_pipestage *ic = &processor->ic;

  ic->address = processor->pc;

  if (r3051_icache_fetch(ic->address, &ic->code) == false) {
    assert(0 && "I-Cache Error");
    return;
  }

  processor->pc += 4;
}
