#include "stdafx.h"
#include "r3051.h"
#include "cp0.h"

#define op(name) static void r3051_op_##name(struct r3051* processor, struct r3051_pipestage* stage)

op(add);
op(addi);
op(addiu);
op(addu);
op(and);
op(andi);
op(beq);
op(bne);
op(blez);
op(bgtz);
op(j);
op(jal);
op(jalr);
op(jr);
op(lb);
op(lbu);
op(lui);
op(lw);
op(or);
op(ori);
op(sb);
op(sh);
op(sll);
op(sltu);
op(sw);

static void r3051_op_mfc(struct r3051*, struct r3051_pipestage*, uint32_t);
static void r3051_op_mtc(struct r3051*, struct r3051_pipestage*, uint32_t);

void r3051_stage_ex_00(struct r3051* processor, struct r3051_pipestage* stage) {
  struct r3051_pipestage* ex = &processor->ex;

  switch (ex->fn) {
  case 0x00: r3051_op_sll(processor, ex); return;
  case 0x08: r3051_op_jr(processor, ex); return;
  case 0x09: r3051_op_jalr(processor, ex); return;
  case 0x20: r3051_op_add(processor, ex); return;
  case 0x21: r3051_op_addu(processor, ex); return;
  case 0x24: r3051_op_and(processor, ex); return;
  case 0x25: r3051_op_or(processor, ex); return;
  case 0x2b: r3051_op_sltu(processor, ex); return;
  }

  assert(0 && "Unimplemented instruction");
}

void r3051_stage_ex_cp(struct r3051* processor, struct r3051_pipestage* stage) {
  if (stage->code & (1 << 25)) {
    assert(0 && "Unimplemented coprocessor instruction");
    return;
  }

  uint32_t cp = stage->op & 3;

  switch (stage->rs) {
  case 0x00: r3051_op_mfc(processor, stage, cp); return;
  case 0x04: r3051_op_mtc(processor, stage, cp); return;
  }

  assert(0 && "Unimplemented instruction");
}

void r3051_stage_ex(struct r3051* processor) {
  struct r3051_pipestage* stage = &processor->ex;

  switch (stage->op) {
  case 0x00: r3051_stage_ex_00(processor, stage); return;
  case 0x02: r3051_op_j(processor, stage); return;
  case 0x03: r3051_op_jal(processor, stage); return;
  case 0x04: r3051_op_beq(processor, stage); return;
  case 0x05: r3051_op_bne(processor, stage); return;
  case 0x06: r3051_op_blez(processor, stage); return;
  case 0x07: r3051_op_bgtz(processor, stage); return;
  case 0x08: r3051_op_addi(processor, stage); return;
  case 0x09: r3051_op_addiu(processor, stage); return;
  case 0x0c: r3051_op_andi(processor, stage); return;
  case 0x0d: r3051_op_ori(processor, stage); return;
  case 0x0f: r3051_op_lui(processor, stage); return;
  case 0x10: r3051_stage_ex_cp(processor, stage); return;
  case 0x11: r3051_stage_ex_cp(processor, stage); return;
  case 0x12: r3051_stage_ex_cp(processor, stage); return;
  case 0x13: r3051_stage_ex_cp(processor, stage); return;
  case 0x20: r3051_op_lb(processor, stage); return;
  case 0x23: r3051_op_lw(processor, stage); return;
  case 0x24: r3051_op_lbu(processor, stage); return;
  case 0x28: r3051_op_sb(processor, stage); return;
  case 0x29: r3051_op_sh(processor, stage); return;
  case 0x2b: r3051_op_sw(processor, stage); return;
  }

  assert(0 && "Unimplemented instruction");
}

op(add) {
  uint32_t result = processor->registers[stage->rs] + processor->registers[stage->rt];

  // TODO: arithmetic overflow exception

  processor->registers[stage->rd] = result;
}

op(addi) {
  uint32_t result = processor->registers[stage->rs] + ((int16_t) stage->si);

  // TODO: arithmetic overflow exception

  processor->registers[stage->rt] = result;
}

op(addiu) {
  processor->registers[stage->rt] = processor->registers[stage->rs] + ((int16_t) stage->si);
}

op(addu) {
  processor->registers[stage->rd] = processor->registers[stage->rs] + processor->registers[stage->rt];
}

op(and) {
  processor->registers[stage->rt] = processor->registers[stage->rs] & processor->registers[stage->rt];
}

op(andi) {
  processor->registers[stage->rt] = processor->registers[stage->rs] & stage->si;
}

op(beq) {
  if (processor->registers[stage->rs] == processor->registers[stage->rt]) {
    processor->pc -= 4;
    processor->pc += ((int16_t) stage->si) << 2;
  }
}

op(bne) {
  if (processor->registers[stage->rs] != processor->registers[stage->rt]) {
    processor->pc -= 4;
    processor->pc += ((int16_t) stage->si) << 2;
  }
}

op(bgtz) {
  if (((int32_t)processor->registers[stage->rs]) > 0) {
    processor->pc -= 4;
    processor->pc += ((int16_t) stage->si) << 2;
  }
}

op(blez) {
  if (((int32_t)processor->registers[stage->rs]) <= 0) {
    processor->pc -= 4;
    processor->pc += ((int16_t)stage->si) << 2;
  }
}

op(j) {
  processor->pc -= 4;
  processor->pc &= 0xf0000000;
  processor->pc |= stage->si << 2;
}

op(jal) {
  processor->registers[31] = processor->pc;

  processor->pc -= 4;
  processor->pc &= 0xf0000000;
  processor->pc |= stage->si << 2;
}

op(jalr) {
  processor->registers[stage->rd] = processor->pc;

  processor->pc = processor->registers[stage->rs];
}

op(jr) {
  processor->pc = processor->registers[stage->rs];
}

op(lb) {
  stage->target = processor->registers[stage->rs] + ((int16_t) stage->si);
}

op(lbu) {
  stage->target = processor->registers[stage->rs] + ((int16_t) stage->si);
}

op(lui) {
  processor->registers[stage->rt] = stage->si << 16;
}

op(lw) {
  stage->target = processor->registers[stage->rs] + ((int16_t) stage->si);
}

op(or) {
  processor->registers[stage->rd] = processor->registers[stage->rs] | processor->registers[stage->rt];
}

op(ori) {
  processor->registers[stage->rt] = processor->registers[stage->rs] | stage->si;
}

op(sb) {
  stage->target = processor->registers[stage->rs] + ((int16_t) stage->si);
}

op(sh) {
  stage->target = processor->registers[stage->rs] + ((int16_t) stage->si);
}

op(sll) {
  processor->registers[stage->rd] = processor->registers[stage->rt] << stage->sh;
}

op(sltu) {
  processor->registers[stage->rd] = processor->registers[stage->rs] < processor->registers[stage->rt];
}

op(sw) {
  stage->target = processor->registers[stage->rs] + ((int16_t) stage->si);
}

// coprocessor instructions

extern struct r3051_cp0* cp0;

static void r3051_op_mfc(struct r3051* processor, struct r3051_pipestage* stage, uint32_t cp) {
  switch (cp) {
  case 0x00: r3051_cp0_fetch(cp0, stage->rd, &processor->registers[stage->rt]); return;
  case 0x01: break;
  case 0x02: break;
  case 0x03: break;
  }

  assert(0 && "Unimplemented coprocessor");
}

static void r3051_op_mtc(struct r3051* processor, struct r3051_pipestage* stage, uint32_t cp) {
  switch (cp) {
  case 0x00: r3051_cp0_store(cp0, stage->rd, &processor->registers[stage->rt]); return;
  case 0x01: break;
  case 0x02: break;
  case 0x03: break;
  }

  assert(0 && "Unimplemented coprocessor");
}
