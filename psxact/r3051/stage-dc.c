#include "stdafx.h"
#include "r3051.h"

#define op(name) static void r3051_op_##name(struct r3051* processor, struct r3051_pipestage* stage)

op(lw);
op(sb);
op(sh);
op(sw);

void r3051_stage_dc_00(struct r3051* processor, struct r3051_pipestage* stage) {
  switch ((stage->code >> 0) & 63) {
  case 0x00: return;
  case 0x08: return;
  case 0x21: return;
  case 0x25: return;
  case 0x2b: return;
  }

  assert(0 && "Unimplemented instruction");
}

void r3051_stage_dc(struct r3051* processor) {
  struct r3051_pipestage* dc = &processor->dc;

  switch ((dc->code >> 26) & 63) {
  case 0x00: r3051_stage_dc_00(processor, dc); return;
  case 0x02: return;
  case 0x03: return;
  case 0x05: return;
  case 0x08: return;
  case 0x09: return;
  case 0x0c: return;
  case 0x0d: return;
  case 0x0f: return;
  case 0x10: return;
  case 0x23: r3051_op_lw(processor, dc); return;
  case 0x28: r3051_op_sb(processor, dc); return;
  case 0x29: r3051_op_sh(processor, dc); return;
  case 0x2b: r3051_op_sw(processor, dc); return;
  }

  assert(0 && "Unimplemented instruction");
}

op(lw) {
  if (r3051_dcache_fetch(stage->target, &processor->registers[stage->rt])) {
    assert(0 && "D-Cache Error");
  }
}

op(sb) {
  if (r3051_dcache_store(stage->target, &processor->registers[stage->rt])) {
    assert(0 && "D-Cache Error");
  }
}

op(sh) {
  if (r3051_dcache_store(stage->target, &processor->registers[stage->rt])) {
    assert(0 && "D-Cache Error");
  }
}

op(sw) {
  if (r3051_dcache_store(stage->target, &processor->registers[stage->rt])) {
    assert(0 && "D-Cache Error");
  }
}
