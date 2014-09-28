#include "stdafx.h"
#include "r3051.h"

#define DECODE_JT() ((stage->code >>  0) & 0x3ffffff)
#define DECODE_SI() ((stage->code >>  0) & 0xffff)
#define DECODE_FN() ((stage->code >>  0) & 63)
#define DECODE_SA() ((stage->code >>  6) & 31)
#define DECODE_RD() ((stage->code >> 11) & 31)
#define DECODE_RT() ((stage->code >> 16) & 31)
#define DECODE_RS() ((stage->code >> 21) & 31)
#define DECODE_OP() ((stage->code >> 26) & 63)

static void decode_itype(struct r3051*, struct r3051_stage*);
static void decode_jtype(struct r3051*, struct r3051_stage*);
static void decode_rtype(struct r3051*, struct r3051_stage*);

static void r3051_stage_rf_00(struct r3051* processor, struct r3051_stage* stage) {
  switch (stage->fn = DECODE_FN()) {
  case 0x00: decode_rtype(processor, stage); return; // sll rd,rt,sa
//case 0x01:
  case 0x02: decode_rtype(processor, stage); return; // srl rd,rt,sa
  case 0x03: decode_rtype(processor, stage); return; // sra rd,rt,sa
  case 0x04: decode_rtype(processor, stage); return; // sllv rd,rt,rs
//case 0x05:
  case 0x06: decode_rtype(processor, stage); return; // srlv rd,rt,rs
  case 0x07: decode_rtype(processor, stage); return; // srav rd,rt,rs
  case 0x08: decode_rtype(processor, stage); return; // jr rs
  case 0x09: decode_rtype(processor, stage); return; // jalr rd,rs
//case 0x0a:
//case 0x0b:
  case 0x0c: return; // syscall
  case 0x0d: return; // break
//case 0x0e:
//case 0x0f:
  case 0x10: decode_rtype(processor, stage); return; // mfhi rd
  case 0x11: decode_rtype(processor, stage); return; // mthi rs
  case 0x12: decode_rtype(processor, stage); return; // mflo rd
  case 0x13: decode_rtype(processor, stage); return; // mtlo rs
//case 0x14:
//case 0x15:
//case 0x16:
//case 0x17:
  case 0x18: decode_rtype(processor, stage); return; // mult rs,rt
  case 0x19: decode_rtype(processor, stage); return; // multu rs,rt
  case 0x1a: decode_rtype(processor, stage); return; // div rs,rt
  case 0x1b: decode_rtype(processor, stage); return; // divu rs,rt
//case 0x1c:
//case 0x1d:
//case 0x1e:
//case 0x1f:
  case 0x20: decode_rtype(processor, stage); return; // add rd,rt,rs
  case 0x21: decode_rtype(processor, stage); return; // addu rd,rt,rs
  case 0x22: decode_rtype(processor, stage); return; // sub rd,rt,rs
  case 0x23: decode_rtype(processor, stage); return; // subu rd,rt,rs
  case 0x24: decode_rtype(processor, stage); return; // and rd,rt,rs
  case 0x25: decode_rtype(processor, stage); return; // or rd,rt,rs
  case 0x26: decode_rtype(processor, stage); return; // xor rd,rt,rs
  case 0x27: decode_rtype(processor, stage); return; // nor rd,rt,rs
//case 0x28:
//case 0x29:
  case 0x2a: decode_rtype(processor, stage); return; // slt rd,rt,rs
  case 0x2b: decode_rtype(processor, stage); return; // sltu rd,rt,rs
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

static void r3051_stage_rf_01(struct r3051* processor, struct r3051_stage* stage) {
  switch (stage->rt = DECODE_RT()) {
  case 0x00: decode_itype(processor, stage); return; // bltz rs,immediate
  case 0x01: decode_itype(processor, stage); return; // bgez rs,immediate
//case 0x02:
//case 0x03:
//case 0x04:
//case 0x05:
//case 0x06:
//case 0x07:
//case 0x08:
//case 0x09:
//case 0x0a:
//case 0x0b:
//case 0x0c:
//case 0x0d:
//case 0x0e:
//case 0x0f:
  case 0x10: decode_itype(processor, stage); return; // bltzal rs,immediate
  case 0x11: decode_itype(processor, stage); return; // bgezal rs,immediate
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
  }

  assert(0 && "Unimplemented instruction");
}

void r3051_stage_rf(struct r3051* processor) {
  struct r3051_stage* stage = &processor->rf;

  switch (stage->op = DECODE_OP()) {
  case 0x00: r3051_stage_rf_00(processor, stage); return;
  case 0x01: r3051_stage_rf_01(processor, stage); return;
  case 0x02: decode_jtype(processor, stage); return; // j immediate
  case 0x03: decode_jtype(processor, stage); return; // jal immediate
  case 0x04: decode_itype(processor, stage); return; // beq rt,rs,immediate
  case 0x05: decode_itype(processor, stage); return; // bne rt,rs,immediate
  case 0x06: decode_itype(processor, stage); return; // blez rs,immediate
  case 0x07: decode_itype(processor, stage); return; // bgtz rs,immediate
  case 0x08: decode_itype(processor, stage); return; // addi rt,rs,immediate
  case 0x09: decode_itype(processor, stage); return; // addiu rt,rs,immediate
  case 0x0a: decode_itype(processor, stage); return; // slti rt,rs,immediate
  case 0x0b: decode_itype(processor, stage); return; // sltiu rt,rs,immediate
  case 0x0c: decode_itype(processor, stage); return; // andi rt,rs,immediate
  case 0x0d: decode_itype(processor, stage); return; // ori rt,rs,immediate
  case 0x0e: decode_itype(processor, stage); return; // xori rt,rs,immediate
  case 0x0f: decode_itype(processor, stage); return; // lui rt,immediate
  case 0x10: decode_rtype(processor, stage); return; // cop0
  case 0x11: decode_rtype(processor, stage); return; // cop1
  case 0x12: decode_rtype(processor, stage); return; // cop2
  case 0x13: decode_rtype(processor, stage); return; // cop3
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
  case 0x20: decode_itype(processor, stage); return; // lb rt,immediate(rs)
  case 0x21: decode_itype(processor, stage); return; // lh rt,immediate(rs)
  case 0x22: decode_itype(processor, stage); return; // lwl rt,immediate(rs)
  case 0x23: decode_itype(processor, stage); return; // lw rt,immediate(rs)
  case 0x24: decode_itype(processor, stage); return; // lbu rt,immediate(rs)
  case 0x25: decode_itype(processor, stage); return; // lhu rt,immediate(rs)
  case 0x26: decode_itype(processor, stage); return; // lwr rt,immediate(rs)
//case 0x27:
  case 0x28: decode_itype(processor, stage); return; // sb rt,immediate(rs)
  case 0x29: decode_itype(processor, stage); return; // sh rt,immediate(rs)
  case 0x2a: decode_itype(processor, stage); return; // swl rt,immediate(rs)
  case 0x2b: decode_itype(processor, stage); return; // sw rt,immediate(rs)
//case 0x2c:
//case 0x2d:
  case 0x2e: decode_itype(processor, stage); return; // swr rt,immediate(rs)
//case 0x2f:
  case 0x30: decode_itype(processor, stage); return; // lwc0 rt,immediate(rs)
  case 0x31: decode_itype(processor, stage); return; // lwc1 rt,immediate(rs)
  case 0x32: decode_itype(processor, stage); return; // lwc2 rt,immediate(rs)
  case 0x33: decode_itype(processor, stage); return; // lwc3 rt,immediate(rs)
//case 0x34:
//case 0x35:
//case 0x36:
//case 0x37:
  case 0x38: decode_itype(processor, stage); return; // swc0 rt,immediate(rs)
  case 0x39: decode_itype(processor, stage); return; // swc1 rt,immediate(rs)
  case 0x3a: decode_itype(processor, stage); return; // swc2 rt,immediate(rs)
  case 0x3b: decode_itype(processor, stage); return; // swc3 rt,immediate(rs)
//case 0x3c:
//case 0x3d:
//case 0x3e:
//case 0x3f:
  }

  assert(0 && "Unimplemented instruction");
}

static void decode_itype(struct r3051* processor, struct r3051_stage* stage) {
  stage->nn = DECODE_SI();
  stage->rt = DECODE_RT();
  stage->rs = DECODE_RS();
}

static void decode_jtype(struct r3051* processor, struct r3051_stage* stage) {
  stage->nn = DECODE_JT();
}

static void decode_rtype(struct r3051* processor, struct r3051_stage* stage) {
  stage->sa = DECODE_SA();
  stage->rd = DECODE_RD();
  stage->rt = DECODE_RT();
  stage->rs = DECODE_RS();
}
