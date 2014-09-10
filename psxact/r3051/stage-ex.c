#include "stdafx.h"
#include "r3051.h"
#include "cop0.h"

#define ExLog(format, ...) printf(format, __VA_ARGS__)

#define op_impl(name) static void r3051_stage_ex_##name(struct r3051*, struct r3051_stage*)
#define op_decl(name) static void r3051_stage_ex_##name(struct r3051* processor, struct r3051_stage* stage)
#define op_call(name) r3051_stage_ex_##name(processor, stage)

op_impl(add);
op_impl(addi);
op_impl(addiu);
op_impl(addu);
op_impl(and);
op_impl(andi);
op_impl(beq);
op_impl(bgez);
op_impl(bgtz);
op_impl(blez);
op_impl(bltz);
op_impl(bne);
op_impl(div);
op_impl(divu);
op_impl(j);
op_impl(jal);
op_impl(jalr);
op_impl(jr);
op_impl(lui);
op_impl(lb);
op_impl(lbu);
op_impl(lw);
op_impl(mfhi);
op_impl(mflo);
op_impl(or);
op_impl(ori);
op_impl(sll);
op_impl(slt);
op_impl(slti);
op_impl(sltiu);
op_impl(sltu);
op_impl(sra);
op_impl(srl);
op_impl(sub);
op_impl(subu);
op_impl(sb);
op_impl(sh);
op_impl(sw);
op_impl(xor);
op_impl(xori);

extern struct r3051_cop0* cop0;

static void r3051_stage_ex_00(struct r3051* processor, struct r3051_stage* stage) {
  switch (stage->fn) {
  case 0x00: op_call(sll); return;

  case 0x02: op_call(srl); return;
  case 0x03: op_call(sra); return;

  case 0x08: op_call(jr); return;
  case 0x09: op_call(jalr); return;

  case 0x10: op_call(mfhi); return;

  case 0x12: op_call(mflo); return;

  case 0x1a: op_call(div); return;
  case 0x1b: op_call(divu); return;

  case 0x20: op_call(add); return;
  case 0x21: op_call(addu); return;
  case 0x22: op_call(sub); return;
  case 0x23: op_call(subu); return;
  case 0x24: op_call(and); return;
  case 0x25: op_call(or); return;
  case 0x26: op_call(xor); return;

  case 0x2a: op_call(slt); return;
  case 0x2b: op_call(sltu); return;
  }

  assert(0 && "Unimplemented instruction");
}

static void r3051_stage_ex_01(struct r3051* processor, struct r3051_stage* stage) {
  switch (stage->fn) {
  case 0x00: op_call(bltz); return;
  case 0x01: op_call(bgez); return;
  }

  assert(0 && "Unimplemented instruction");
}

static void r3051_stage_ex_cp(struct r3051* processor, struct r3051_stage* stage) {
  switch (stage->op & 3) {
  case 0:
    switch (stage->rs) {
    case 0x00: processor->registers[stage->rt] = r3051_cop0_fetch_sr(cop0, stage->rd); return; // mfc0 rd,rt
    case 0x02: processor->registers[stage->rt] = r3051_cop0_fetch_cr(cop0, stage->rd); return; // cfc0 rd,rt
    case 0x04: r3051_cop0_store_sr(cop0, stage->rd, processor->registers[stage->rt]); return; // mtc0 rd,rt
    case 0x06: r3051_cop0_store_cr(cop0, stage->rd, processor->registers[stage->rt]); return; // ctc0 rd,rt
    }
    break;
  }

  assert(0 && "Unimplemented coprocessor instruction");
}

void r3051_stage_ex(struct r3051* processor) {
  struct r3051_stage* stage = &processor->ex;

  switch (stage->op) {
  case 0x00: r3051_stage_ex_00(processor, stage); return;
  case 0x01: r3051_stage_ex_01(processor, stage); return;
  case 0x02: op_call(j); return;
  case 0x03: op_call(jal); return;
  case 0x04: op_call(beq); return;
  case 0x05: op_call(bne); return;
  case 0x06: op_call(blez); return;
  case 0x07: op_call(bgtz); return;
  case 0x08: op_call(addi); return;
  case 0x09: op_call(addiu); return;
  case 0x0a: op_call(slti); return;
  case 0x0b: op_call(sltiu); return;
  case 0x0c: op_call(andi); return;
  case 0x0d: op_call(ori); return;
  case 0x0e: op_call(xori); return;
  case 0x0f: op_call(lui); return;
  case 0x10: op_call(cp); return;
//case 0x11:
//case 0x12:
//case 0x13:
//case 0x14:
//case 0x15:
//case 0x16:
//case 0x17:
//case 0x18:
//case 0x19:
//case 0x1a:
//case 0x1b:
//case 0x1c:
//case 0x1d:
//case 0x1e:
//case 0x1f:
  case 0x20: op_call(lb); return;
//case 0x21:
//case 0x22:
  case 0x23: op_call(lw); return;
  case 0x24: op_call(lbu); return;
//case 0x25:
//case 0x26:
//case 0x27:
  case 0x28: op_call(sb); return;
  case 0x29: op_call(sh); return;
//case 0x2a:
  case 0x2b: op_call(sw); return;
//case 0x2c:
//case 0x2d:
//case 0x2e:
//case 0x2f:
//case 0x30:
//case 0x31:
//case 0x32:
//case 0x33:
//case 0x34:
//case 0x35:
//case 0x36:
//case 0x37:
//case 0x38:
//case 0x39:
//case 0x3a:
//case 0x3b:
//case 0x3c:
//case 0x3d:
//case 0x3e:
//case 0x3f:
  }

  assert(0 && "Unimplemented instruction");
}

op_decl(add) {
  uint32_t sum = processor->registers[stage->rs] + processor->registers[stage->rt];

  // todo: integer overflow exception

  processor->registers[stage->rd] = sum;
}

op_decl(addi) {
  uint32_t sum = processor->registers[stage->rs] + ((int16_t) stage->nn);

  // todo: integer overflow exception

  processor->registers[stage->rt] = sum;
}

op_decl(addiu) {
  processor->registers[stage->rt] = processor->registers[stage->rs] + ((int16_t) stage->nn);
}

op_decl(addu) {
  processor->registers[stage->rd] = processor->registers[stage->rs] + processor->registers[stage->rt];
}

op_decl(and) {
  processor->registers[stage->rd] = processor->registers[stage->rs] & processor->registers[stage->rt];
}

op_decl(andi) {
  processor->registers[stage->rt] = processor->registers[stage->rs] & stage->nn;
}

op_decl(beq) {
  if (processor->registers[stage->rt] == processor->registers[stage->rs]) {
    processor->pc -= 4;
    processor->pc += (uint32_t) (((int16_t) stage->nn) << 2);
  }
}

op_decl(bgez) {
  if (((int32_t)processor->registers[stage->rs]) >= 0) {
    processor->pc -= 4;
    processor->pc += (uint32_t) (((int16_t) stage->nn) << 2);
  }
}

op_decl(bgtz) {
  if (((int32_t) processor->registers[stage->rs]) > 0) {
    processor->pc -= 4;
    processor->pc += (uint32_t) (((int16_t) stage->nn) << 2);
  }
}

op_decl(blez) {
  if (((int32_t)processor->registers[stage->rs]) <= 0) {
    processor->pc -= 4;
    processor->pc += (uint32_t) (((int16_t) stage->nn) << 2);
  }
}

op_decl(bltz) {
  if (((int32_t)processor->registers[stage->rs]) < 0) {
    processor->pc -= 4;
    processor->pc += (uint32_t) (((int16_t) stage->nn) << 2);
  }
}

op_decl(bne) {
  if (processor->registers[stage->rt] != processor->registers[stage->rs]) {
    processor->pc -= 4;
    processor->pc += (uint32_t) (((int16_t) stage->nn) << 2);
  }
}

op_decl(div) {
  int32_t rs = ((int32_t) processor->registers[stage->rs]);
  int32_t rt = ((int32_t) processor->registers[stage->rt]);

  if (rt == 0) {
    processor->lo = -((rs >> 31) | 1);
    processor->hi = rs;
  }
  else {
    processor->lo = rs / rt;
    processor->hi = rs % rt;
  }
}

op_decl(divu) {
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

op_decl(j) {
  processor->pc -= 4;
  processor->pc &= 0xf0000000;
  processor->pc |= stage->nn << 2;

  ExLog("[$%08x] j $%08x\n", stage->address, processor->pc);
}

op_decl(jal) {
  processor->registers[31] = processor->pc;

  processor->pc -= 4;
  processor->pc &= 0xf0000000;
  processor->pc |= stage->nn << 2;

  ExLog("[$%08x] jal $%08x\n", stage->address, processor->pc);
}

op_decl(jalr) {
  processor->registers[stage->rd] = processor->pc;

  processor->pc = processor->registers[stage->rs];

  ExLog("[$%08x] jalr r%d,r%d\n", stage->address, stage->rd, stage->rs);
}

op_decl(jr) {
  processor->pc = processor->registers[stage->rs];

  ExLog("[$%08x] jr r%d\n", stage->address, stage->rs);
}

op_decl(lui) {
  processor->registers[stage->rt] = stage->nn << 16;
}

op_decl(lb) {
  stage->target = processor->registers[stage->rs] + ((int16_t) stage->nn);
}

op_decl(lbu) {
  stage->target = processor->registers[stage->rs] + ((int16_t) stage->nn);
}

op_decl(lw) {
  stage->target = processor->registers[stage->rs] + ((int16_t) stage->nn);
}

op_decl(mfhi) {
  processor->registers[stage->rd] = processor->hi;
}

op_decl(mflo) {
  processor->registers[stage->rd] = processor->lo;
}

op_decl(or) {
  processor->registers[stage->rd] = processor->registers[stage->rs] | processor->registers[stage->rt];
}

op_decl(ori) {
  processor->registers[stage->rt] = processor->registers[stage->rs] | stage->nn;
}

op_decl(sll) {
  processor->registers[stage->rd] = processor->registers[stage->rt] << stage->sa;
}

op_decl(slt) {
  processor->registers[stage->rd] =
    ((int32_t) processor->registers[stage->rs]) < ((int32_t) processor->registers[stage->rt]);
}

op_decl(slti) {
  processor->registers[stage->rt] = 
    ((int32_t) processor->registers[stage->rs]) < ((int16_t) stage->nn);
}

op_decl(sltiu) {
  processor->registers[stage->rt] = processor->registers[stage->rs] < ((uint32_t) ((int16_t) stage->nn));
}

op_decl(sltu) {
  processor->registers[stage->rd] = processor->registers[stage->rs] < processor->registers[stage->rt];
}

op_decl(sra) {
  processor->registers[stage->rd] = ((int32_t) processor->registers[stage->rt]) >> stage->sa;
}

op_decl(srl) {
  processor->registers[stage->rd] = processor->registers[stage->rt] >> stage->sa;
}

op_decl(sub) {
  uint32_t difference = processor->registers[stage->rs] - processor->registers[stage->rt];

  // todo: arithmetic overflow exception

  processor->registers[stage->rd] = difference;
}

op_decl(subu) {
  processor->registers[stage->rd] = processor->registers[stage->rs] - processor->registers[stage->rt];
}

op_decl(sb) {
  stage->target = processor->registers[stage->rs] + ((int16_t) stage->nn);
}

op_decl(sh) {
  stage->target = processor->registers[stage->rs] + ((int16_t) stage->nn);
}

op_decl(sw) {
  stage->target = processor->registers[stage->rs] + ((int16_t) stage->nn);
}

op_decl(xor) {
  processor->registers[stage->rd] = processor->registers[stage->rs] ^ processor->registers[stage->rt];
}

op_decl(xori) {
  processor->registers[stage->rt] = processor->registers[stage->rs] ^ stage->nn;
}
