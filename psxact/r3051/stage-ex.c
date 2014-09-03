#include "stdafx.h"
#include "r3051.h"

#define op(name) static void r3051_op_##name(struct r3051* processor, struct r3051_pipestage* stage)

op(add);
op(addi);
op(addiu);
op(addu);
op(and);
op(andi);
op(beq);
op(bne);
op(j);
op(jal);
op(jr);
op(lb);
op(lui);
op(lw);
op(or);
op(ori);
op(sb);
op(sh);
op(sll);
op(sltu);
op(sw);

void r3051_stage_ex_00(struct r3051* processor, struct r3051_pipestage* stage) {
  struct r3051_pipestage* ex = &processor->ex;

  switch (ex->fn) {
  case 0x00: r3051_op_sll(processor, ex); return;
  case 0x08: r3051_op_jr(processor, ex); return;
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
  }
}

void r3051_stage_ex(struct r3051* processor) {
  struct r3051_pipestage* ex = &processor->ex;

  switch (ex->op) {
  case 0x00: r3051_stage_ex_00(processor, ex); return;
  case 0x02: r3051_op_j(processor, ex); return;
  case 0x03: r3051_op_jal(processor, ex); return;
  case 0x04: r3051_op_beq(processor, ex); return;
  case 0x05: r3051_op_bne(processor, ex); return;
  case 0x08: r3051_op_addi(processor, ex); return;
  case 0x09: r3051_op_addiu(processor, ex); return;
  case 0x0c: r3051_op_andi(processor, ex); return;
  case 0x0d: r3051_op_ori(processor, ex); return;
  case 0x0f: r3051_op_lui(processor, ex); return;
  case 0x10: r3051_stage_ex_cp(processor, ex); return;
  case 0x11: r3051_stage_ex_cp(processor, ex); return;
  case 0x12: r3051_stage_ex_cp(processor, ex); return;
  case 0x13: r3051_stage_ex_cp(processor, ex); return;
  case 0x20: r3051_op_lb(processor, ex); return;
  case 0x23: r3051_op_lw(processor, ex); return;
  case 0x28: r3051_op_sb(processor, ex); return;
  case 0x29: r3051_op_sh(processor, ex); return;
  case 0x2b: r3051_op_sw(processor, ex); return;
  }

  assert(0 && "Unimplemented instruction");
}

op(add) {
  uint32_t result = processor->registers[stage->rs] + processor->registers[stage->rt];

  processor->registers[stage->rd] = result;
}

op(addi) {
  uint32_t result;

  result = processor->registers[stage->rs] + ((int16_t) stage->si);

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

op(j) {
  processor->pc -= 4;
  processor->pc &= 0xf0000000;
  processor->pc |= stage->si << 2;
}

op(jal) {
  processor->registers[31] = processor->pc;

  processor->pc -= 4; // address of the delay slot
  processor->pc &= 0xf0000000;
  processor->pc |= stage->si << 2;
}

op(jr) {
  processor->pc = processor->registers[stage->rs];
}

op(lb) {
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
  processor->registers[stage->rd] =
    ((uint32_t)processor->registers[stage->rs]) <
    ((uint32_t)processor->registers[stage->rt]);
}

op(sw) {
  stage->target = processor->registers[stage->rs] + ((int16_t) stage->si);
}
