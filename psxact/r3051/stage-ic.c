#include "stdafx.h"
#include "r3051.h"

void r3051_stage_ic(struct r3051* processor) {
  struct r3051_pipestage *ic = &processor->ic;

  ic->address = processor->pc;

  r3051_fetch_inst(WORD, ic->address, &ic->code);

  processor->pc += 4;
}
