#include "stdafx.h"
#include "r3051.h"

#define DECODE_SI() (stage->code >> 0) & 0xffff;
#define DECODE_RT() (stage->code >> 0) & 31;

STAGE(rf) {
  struct r3051_stage* stage = &processor->rf;

  uint32_t a = DECODE_RT();
}
