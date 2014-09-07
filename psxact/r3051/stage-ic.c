#include "stdafx.h"
#include "r3051.h"

void r3051_stage_ic(struct r3051* processor) {
  struct r3051_stage *ic = &processor->ic;

  ic->address = processor->pc;
  ic->code = r3051_fetch_inst(ic->address);

  processor->pc += 4;
}
