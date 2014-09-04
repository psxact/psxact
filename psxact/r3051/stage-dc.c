#include "stdafx.h"
#include "r3051.h"

#define op(name) static void r3051_op_##name(struct r3051* processor, struct r3051_pipestage* stage)

op(lb);
op(lbu);
op(lw);
op(sb);
op(sh);
op(sw);

void r3051_stage_dc(struct r3051* processor) {
  struct r3051_pipestage* stage = &processor->dc;

  switch (stage->op) {
  case 0x00: return;
  case 0x02: return;
  case 0x03: return;
  case 0x04: return;
  case 0x05: return;
  case 0x06: return;
  case 0x07: return;
  case 0x08: return;
  case 0x09: return;
  case 0x0c: return;
  case 0x0d: return;
  case 0x0f: return;
  case 0x10: return;
  case 0x20: r3051_op_lb(processor, stage); return;
  case 0x23: r3051_op_lw(processor, stage); return;
  case 0x24: r3051_op_lbu(processor, stage); return;
  case 0x28: r3051_op_sb(processor, stage); return;
  case 0x29: r3051_op_sh(processor, stage); return;
  case 0x2b: r3051_op_sw(processor, stage); return;
  }

  assert(0 && "Unimplemented instruction");
}

op(lb) {
  r3051_dcache_fetch(BYTE, stage->target, &processor->registers[stage->rt]);
}

op(lbu) {
  r3051_dcache_fetch(UBYTE, stage->target, &processor->registers[stage->rt]);
}

op(lw) {
  r3051_dcache_fetch(WORD, stage->target, &processor->registers[stage->rt]);
}

op(sb) {
  r3051_dcache_store(BYTE, stage->target, &processor->registers[stage->rt]);
}

op(sh) {
  r3051_dcache_store(HALF, stage->target, &processor->registers[stage->rt]);
}

op(sw) {
  r3051_dcache_store(WORD, stage->target, &processor->registers[stage->rt]);
}
