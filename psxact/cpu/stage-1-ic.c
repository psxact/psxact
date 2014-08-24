#include "stdafx.h"
#include "r3051.h"

STAGE(ic) {
  struct r3051_stage* stage = &processor->ic;
  
  stage->address = processor->registers[31];
  stage->code = 0;

  processor->registers[31] += 4;
}
