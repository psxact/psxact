#include "stdafx.h"
#include "r3051.h"

void R3051::r3051_stage_ic(void) {
  R3051::Stage *stage = &this->ic;

  stage->address = this->pc;
  stage->code = this->fetchInst(stage->address);

  this->pc += 4;
}
