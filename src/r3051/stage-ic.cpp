#include "stdafx.hpp"
#include "r3051.hpp"

void R3051::StageIc(void) {
  R3051::Stage *stage = &this->ic;

  stage->address = this->pc;
  stage->code = this->FetchInst(stage->address);

  this->pc += 4;
}
