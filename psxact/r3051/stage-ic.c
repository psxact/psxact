#include "stdafx.h"
#include "r3051.h"

void r3051_stage_ic(struct r3051* processor) {
  struct r3051_stage *stage = &processor->ic;

  stage->address = processor->pc;
  stage->code = r3051_fetch_inst(stage->address);

  processor->pc += 4;
}
