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

void r3051_stage_rf_00(struct r3051* processor, struct r3051_stage* stage) {
  switch (stage->fn = DECODE_FN()) {
  case 0x00: decode_rtype(processor, stage); return; // sll rd,rt,sa

  case 0x02: decode_rtype(processor, stage); return; // srl rd,rt,sa
  case 0x03: decode_rtype(processor, stage); return; // sra rd,rt,sa

  case 0x08: decode_rtype(processor, stage); return; // jr rs
  case 0x09: decode_rtype(processor, stage); return; // jalr rd,rs

  case 0x0c: return; // syscall
  case 0x0d: return; // break
  case 0x10: decode_rtype(processor, stage); return; // mfhi rd

  case 0x12: decode_rtype(processor, stage); return; // mflo rd

  case 0x1a: decode_rtype(processor, stage); return; // div rs,rt
  case 0x1b: decode_rtype(processor, stage); return; // divu rs,rt

  case 0x20: decode_rtype(processor, stage); return; // add rd,rt,rs
  case 0x21: decode_rtype(processor, stage); return; // addu rd,rt,rs

  case 0x23: decode_rtype(processor, stage); return; // subu rd,rt,rs
  case 0x24: decode_rtype(processor, stage); return; // and rd,rt,rs
  case 0x25: decode_rtype(processor, stage); return; // or rd,rt,rs

  case 0x2a: decode_rtype(processor, stage); return; // slt rd,rt,rs
  case 0x2b: decode_rtype(processor, stage); return; // sltu rd,rt,rs
  }

  assert(0 && "Unimplemented instruction");
}

void r3051_stage_rf_01(struct r3051* processor, struct r3051_stage* stage) {
  switch (stage->rt = DECODE_RT()) {
  case 0x00: decode_itype(processor, stage); return; // bltz rs,immediate
  case 0x01: decode_itype(processor, stage); return; // bgez rs,immediate
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

  case 0x0f: decode_itype(processor, stage); return; // lui rt,immediate
  case 0x10: decode_rtype(processor, stage); return; // cop0

  case 0x20: decode_itype(processor, stage); return; // lb rt,immediate(rs)

  case 0x23: decode_itype(processor, stage); return; // lw rt,immediate(rs)
  case 0x24: decode_itype(processor, stage); return; // lbu rt,immediate(rs)

  case 0x28: decode_itype(processor, stage); return; // sb rt,immediate(rs)
  case 0x29: decode_itype(processor, stage); return; // sh rt,immediate(rs)

  case 0x2b: decode_itype(processor, stage); return; // sw rt,immediate(rs)
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
