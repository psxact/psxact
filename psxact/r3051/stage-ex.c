#include "stdafx.h"
#include "r3051.h"
#include "cp0.h"

static void r3051_ex_add(struct r3051*, struct r3051_stage*);
static void r3051_ex_addi(struct r3051*, struct r3051_stage*);
static void r3051_ex_addiu(struct r3051*, struct r3051_stage*);
static void r3051_ex_addu(struct r3051*, struct r3051_stage*);
static void r3051_ex_and(struct r3051*, struct r3051_stage*);
static void r3051_ex_andi(struct r3051*, struct r3051_stage*);
static void r3051_ex_beq(struct r3051*, struct r3051_stage*);
static void r3051_ex_bgez(struct r3051*, struct r3051_stage*);
static void r3051_ex_bgtz(struct r3051*, struct r3051_stage*);
static void r3051_ex_blez(struct r3051*, struct r3051_stage*);
static void r3051_ex_bltz(struct r3051*, struct r3051_stage*);
static void r3051_ex_bne(struct r3051*, struct r3051_stage*);
static void r3051_ex_div(struct r3051*, struct r3051_stage*);
static void r3051_ex_divu(struct r3051*, struct r3051_stage*);
static void r3051_ex_j(struct r3051*, struct r3051_stage*);
static void r3051_ex_jal(struct r3051*, struct r3051_stage*);
static void r3051_ex_jalr(struct r3051*, struct r3051_stage*);
static void r3051_ex_jr(struct r3051*, struct r3051_stage*);
static void r3051_ex_lui(struct r3051*, struct r3051_stage*);
static void r3051_ex_lb(struct r3051*, struct r3051_stage*);
static void r3051_ex_lbu(struct r3051*, struct r3051_stage*);
static void r3051_ex_lw(struct r3051*, struct r3051_stage*);
static void r3051_ex_mfhi(struct r3051*, struct r3051_stage*);
static void r3051_ex_mflo(struct r3051*, struct r3051_stage*);
static void r3051_ex_or(struct r3051*, struct r3051_stage*);
static void r3051_ex_ori(struct r3051*, struct r3051_stage*);
static void r3051_ex_sll(struct r3051*, struct r3051_stage*);
static void r3051_ex_slt(struct r3051*, struct r3051_stage*);
static void r3051_ex_slti(struct r3051*, struct r3051_stage*);
static void r3051_ex_sltiu(struct r3051*, struct r3051_stage*);
static void r3051_ex_sltu(struct r3051*, struct r3051_stage*);
static void r3051_ex_sra(struct r3051*, struct r3051_stage*);
static void r3051_ex_srl(struct r3051*, struct r3051_stage*);
static void r3051_ex_subu(struct r3051*, struct r3051_stage*);
static void r3051_ex_sb(struct r3051*, struct r3051_stage*);
static void r3051_ex_sh(struct r3051*, struct r3051_stage*);
static void r3051_ex_sw(struct r3051*, struct r3051_stage*);

static void r3051_ex_cp(struct r3051*, struct r3051_stage*);

extern struct r3051_cop0* cop0;

void r3051_stage_ex_00(struct r3051* processor, struct r3051_stage* stage) {
  switch (stage->fn) {
  case 0x00: r3051_ex_sll(processor, stage); return;

  case 0x02: r3051_ex_srl(processor, stage); return;
  case 0x03: r3051_ex_sra(processor, stage); return;

  case 0x08: r3051_ex_jr(processor, stage); return;
  case 0x09: r3051_ex_jalr(processor, stage); return;

  case 0x10: r3051_ex_mfhi(processor, stage); return;

  case 0x12: r3051_ex_mflo(processor, stage); return;

  case 0x1a: r3051_ex_div(processor, stage); return;
  case 0x1b: r3051_ex_divu(processor, stage); return;

  case 0x20: r3051_ex_add(processor, stage); return;
  case 0x21: r3051_ex_addu(processor, stage); return;

  case 0x23: r3051_ex_subu(processor, stage); return;
  case 0x24: r3051_ex_and(processor, stage); return;
  case 0x25: r3051_ex_or(processor, stage); return;

  case 0x2a: r3051_ex_slt(processor, stage); return;
  case 0x2b: r3051_ex_sltu(processor, stage); return;
  }

  assert(0 && "Unimplemented instruction");
}

void r3051_stage_ex_01(struct r3051* processor, struct r3051_stage* stage) {
  switch (stage->fn) {
  case 0x00: r3051_ex_bltz(processor, stage); return;
  case 0x01: r3051_ex_bgez(processor, stage); return;
  }

  assert(0 && "Unimplemented instruction");
}

void r3051_stage_ex(struct r3051* processor) {
  struct r3051_stage* stage = &processor->ex;

  switch (stage->op) {
  case 0x00: r3051_stage_ex_00(processor, stage); return;
  case 0x01: r3051_stage_ex_01(processor, stage); return;
  case 0x02: r3051_ex_j(processor, stage); return;
  case 0x03: r3051_ex_jal(processor, stage); return;
  case 0x04: r3051_ex_beq(processor, stage); return;
  case 0x05: r3051_ex_bne(processor, stage); return;
  case 0x06: r3051_ex_blez(processor, stage); return;
  case 0x07: r3051_ex_bgtz(processor, stage); return;
  case 0x08: r3051_ex_addi(processor, stage); return;
  case 0x09: r3051_ex_addiu(processor, stage); return;
  case 0x0a: r3051_ex_slti(processor, stage); return;
  case 0x0b: r3051_ex_sltiu(processor, stage); return;
  case 0x0c: r3051_ex_andi(processor, stage); return;
  case 0x0d: r3051_ex_ori(processor, stage); return;

  case 0x0f: r3051_ex_lui(processor, stage); return;
  case 0x10: r3051_ex_cp(processor, stage); return;

  case 0x20: r3051_ex_lb(processor, stage); return;

  case 0x23: r3051_ex_lw(processor, stage); return;
  case 0x24: r3051_ex_lbu(processor, stage); return;

  case 0x28: r3051_ex_sb(processor, stage); return;
  case 0x29: r3051_ex_sh(processor, stage); return;

  case 0x2b: r3051_ex_sw(processor, stage); return;
  }

  assert(0 && "Unimplemented instruction");
}

static void r3051_ex_add(struct r3051* processor, struct r3051_stage* stage) {
  uint32_t sum = processor->registers[stage->rs] + processor->registers[stage->rt];

  // todo: integer overflow exception

  processor->registers[stage->rd] = sum;
}

static void r3051_ex_addi(struct r3051* processor, struct r3051_stage* stage) {
  uint32_t sum = processor->registers[stage->rs] + ((int16_t) stage->nn);

  // todo: integer overflow exception

  processor->registers[stage->rt] = sum;
}

static void r3051_ex_addiu(struct r3051* processor, struct r3051_stage* stage) {
  processor->registers[stage->rt] = processor->registers[stage->rs] + ((int16_t)stage->nn);
}

static void r3051_ex_addu(struct r3051* processor, struct r3051_stage* stage) {
  processor->registers[stage->rd] = processor->registers[stage->rs] + processor->registers[stage->rt];
}

static void r3051_ex_and(struct r3051* processor, struct r3051_stage* stage) {
  processor->registers[stage->rd] = processor->registers[stage->rs] & processor->registers[stage->rt];
}

static void r3051_ex_andi(struct r3051* processor, struct r3051_stage* stage) {
  processor->registers[stage->rt] = processor->registers[stage->rs] & stage->nn;
}

static void r3051_ex_beq(struct r3051* processor, struct r3051_stage* stage) {
  if (processor->registers[stage->rt] == processor->registers[stage->rs]) {
    processor->pc -= 4;
    processor->pc += (uint32_t) (((int16_t) stage->nn) << 2);
  }
}

static void r3051_ex_bgez(struct r3051* processor, struct r3051_stage* stage) {
  if (((int32_t)processor->registers[stage->rs]) >= 0) {
    processor->pc -= 4;
    processor->pc += (uint32_t)(((int16_t)stage->nn) << 2);
  }
}

static void r3051_ex_bgtz(struct r3051* processor, struct r3051_stage* stage) {
  if (((int32_t) processor->registers[stage->rs]) > 0) {
    processor->pc -= 4;
    processor->pc += (uint32_t)(((int16_t)stage->nn) << 2);
  }
}

static void r3051_ex_blez(struct r3051* processor, struct r3051_stage* stage) {
  if (((int32_t)processor->registers[stage->rs]) <= 0) {
    processor->pc -= 4;
    processor->pc += (uint32_t)(((int16_t)stage->nn) << 2);
  }
}

static void r3051_ex_bltz(struct r3051* processor, struct r3051_stage* stage) {
  if (((int32_t)processor->registers[stage->rs]) < 0) {
    processor->pc -= 4;
    processor->pc += (uint32_t)(((int16_t)stage->nn) << 2);
  }
}

static void r3051_ex_bne(struct r3051* processor, struct r3051_stage* stage) {
  if (processor->registers[stage->rt] != processor->registers[stage->rs]) {
    processor->pc -= 4;
    processor->pc += (uint32_t) (((int16_t) stage->nn) << 2);
  }
}

static void r3051_ex_cp(struct r3051* processor, struct r3051_stage* stage) {
  switch (stage->op & 3) {
  case 0:
    switch (stage->rs) {
    case 0x00: r3051_cop0_fetch(cop0, stage->rd, &processor->registers[stage->rt]); return;
    case 0x04: r3051_cop0_store(cop0, stage->rd, &processor->registers[stage->rt]); return;
    }
    break;
  }

  assert(0 && "Unimplemented coprocessor instruction");
}

static void r3051_ex_div(struct r3051* processor, struct r3051_stage* stage) {
  int32_t rs = ((int32_t) processor->registers[stage->rs]);
  int32_t rt = ((int32_t) processor->registers[stage->rt]);

  if (rt == 0) {
    processor->lo = -((rs >> 31) | 1);
    processor->hi = rs;
  }
  else if (rt == -1 && rs == -0x80000000) {
    processor->lo = -0x80000000;
    processor->hi = 0;
  }
  else {
    processor->lo = rs / rt;
    processor->hi = rs % rt;
  }
}

static void r3051_ex_divu(struct r3051* processor, struct r3051_stage* stage) {
  uint32_t rs = processor->registers[stage->rs];
  uint32_t rt = processor->registers[stage->rt];

  if (rt == 0) {
    processor->lo = 0xffffffff;
    processor->hi = rs;
  }
  else {
    processor->lo = rs / rt;
    processor->hi = rs % rt;
  }
}

static void r3051_ex_j(struct r3051* processor, struct r3051_stage* stage) {
  processor->pc -= 4;
  processor->pc &= 0xf0000000;
  processor->pc |= stage->nn << 2;
}

static void r3051_ex_jal(struct r3051* processor, struct r3051_stage* stage) {
  processor->registers[31] = processor->pc;

  processor->pc -= 4;
  processor->pc &= 0xf0000000;
  processor->pc |= stage->nn << 2;
}

static void r3051_ex_jalr(struct r3051* processor, struct r3051_stage* stage) {
  processor->registers[stage->rd] = processor->pc;

  processor->pc = processor->registers[stage->rs];
}

static void r3051_ex_jr(struct r3051* processor, struct r3051_stage* stage) {
  processor->pc = processor->registers[stage->rs];
}

static void r3051_ex_lui(struct r3051* processor, struct r3051_stage* stage) {
  processor->registers[stage->rt] = stage->nn << 16;
}

static void r3051_ex_lb(struct r3051* processor, struct r3051_stage* stage) {
  stage->target = processor->registers[stage->rs] + ((int16_t) stage->nn);
}

static void r3051_ex_lbu(struct r3051* processor, struct r3051_stage* stage) {
  stage->target = processor->registers[stage->rs] + ((int16_t)stage->nn);
}

static void r3051_ex_lw(struct r3051* processor, struct r3051_stage* stage) {
  stage->target = processor->registers[stage->rs] + ((int16_t) stage->nn);
}

static void r3051_ex_mfhi(struct r3051* processor, struct r3051_stage* stage) {
  processor->registers[stage->rd] = processor->hi;
}

static void r3051_ex_mflo(struct r3051* processor, struct r3051_stage* stage) {
  processor->registers[stage->rd] = processor->lo;
}

static void r3051_ex_or(struct r3051* processor, struct r3051_stage* stage) {
  processor->registers[stage->rd] = processor->registers[stage->rs] | processor->registers[stage->rt];
}

static void r3051_ex_ori(struct r3051* processor, struct r3051_stage* stage) {
  processor->registers[stage->rt] = processor->registers[stage->rs] | stage->nn;
}

static void r3051_ex_sll(struct r3051* processor, struct r3051_stage* stage) {
  processor->registers[stage->rd] = processor->registers[stage->rt] << stage->sa;
}

static void r3051_ex_slt(struct r3051* processor, struct r3051_stage* stage) {
  processor->registers[stage->rd] =
    ((int32_t) processor->registers[stage->rs]) < ((int32_t) processor->registers[stage->rt]);
}

static void r3051_ex_slti(struct r3051* processor, struct r3051_stage* stage) {
  processor->registers[stage->rt] = 
    ((int32_t) processor->registers[stage->rs]) < ((int16_t) stage->nn);
}

static void r3051_ex_sltiu(struct r3051* processor, struct r3051_stage* stage) {
  processor->registers[stage->rt] = processor->registers[stage->rs] < ((uint32_t) ((int16_t) stage->nn));
}

static void r3051_ex_sltu(struct r3051* processor, struct r3051_stage* stage) {
  processor->registers[stage->rd] = processor->registers[stage->rs] < processor->registers[stage->rt];
}

static void r3051_ex_sra(struct r3051* processor, struct r3051_stage* stage) {
  processor->registers[stage->rd] = ((int32_t) processor->registers[stage->rt]) >> stage->sa;
}

static void r3051_ex_srl(struct r3051* processor, struct r3051_stage* stage) {
  processor->registers[stage->rd] = processor->registers[stage->rt] >> stage->sa;
}

static void r3051_ex_subu(struct r3051* processor, struct r3051_stage* stage) {
  processor->registers[stage->rd] = processor->registers[stage->rs] - processor->registers[stage->rt];
}

static void r3051_ex_sb(struct r3051* processor, struct r3051_stage* stage) {
  stage->target = processor->registers[stage->rs] + ((int16_t)stage->nn);
}

static void r3051_ex_sh(struct r3051* processor, struct r3051_stage* stage) {
  stage->target = processor->registers[stage->rs] + ((int16_t) stage->nn);
}

static void r3051_ex_sw(struct r3051* processor, struct r3051_stage* stage) {
  stage->target = processor->registers[stage->rs] + ((int16_t) stage->nn);
}
