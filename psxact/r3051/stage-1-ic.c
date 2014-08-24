#include "stdafx.h"
#include "r3051.h"

STAGE(ic) {
  struct r3051_pipestage* stage = &processor->ic;
  
  stage->address = processor->pc;
  stage->code = 0; // replace with instruction cache fetching

  processor->pc += 4;
}
