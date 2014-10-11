#include "stdafx.hpp"
#include "r3051.hpp"

#define DECODE_JT() Util::Mask<26>(stage->code >>  0)
#define DECODE_SI() Util::Mask<16>(stage->code >>  0)
#define DECODE_FN() Util::Mask< 6>(stage->code >>  0)
#define DECODE_SA() Util::Mask< 5>(stage->code >>  6)
#define DECODE_RD() Util::Mask< 5>(stage->code >> 11)
#define DECODE_RT() Util::Mask< 5>(stage->code >> 16)
#define DECODE_RS() Util::Mask< 5>(stage->code >> 21)
#define DECODE_OP() Util::Mask< 6>(stage->code >> 26)

static void DecodeIType(R3051*, R3051::Stage*);
static void DecodeJType(R3051*, R3051::Stage*);
static void DecodeRType(R3051*, R3051::Stage*);

static void StageRf00(R3051* processor, R3051::Stage* stage) {
  switch (stage->fn = DECODE_FN()) {
  case 0x00: DecodeRType(processor, stage); return; // sll rd,rt,sa
//case 0x01:
  case 0x02: DecodeRType(processor, stage); return; // srl rd,rt,sa
  case 0x03: DecodeRType(processor, stage); return; // sra rd,rt,sa
  case 0x04: DecodeRType(processor, stage); return; // sllv rd,rt,rs
//case 0x05:
  case 0x06: DecodeRType(processor, stage); return; // srlv rd,rt,rs
  case 0x07: DecodeRType(processor, stage); return; // srav rd,rt,rs
  case 0x08: DecodeRType(processor, stage); return; // jr rs
  case 0x09: DecodeRType(processor, stage); return; // jalr rd,rs
//case 0x0a:
//case 0x0b:
  case 0x0c: return; // syscall
  case 0x0d: return; // break
//case 0x0e:
//case 0x0f:
  case 0x10: DecodeRType(processor, stage); return; // mfhi rd
  case 0x11: DecodeRType(processor, stage); return; // mthi rs
  case 0x12: DecodeRType(processor, stage); return; // mflo rd
  case 0x13: DecodeRType(processor, stage); return; // mtlo rs
//case 0x14:
//case 0x15:
//case 0x16:
//case 0x17:
  case 0x18: DecodeRType(processor, stage); return; // mult rs,rt
  case 0x19: DecodeRType(processor, stage); return; // multu rs,rt
  case 0x1a: DecodeRType(processor, stage); return; // div rs,rt
  case 0x1b: DecodeRType(processor, stage); return; // divu rs,rt
//case 0x1c:
//case 0x1d:
//case 0x1e:
//case 0x1f:
  case 0x20: DecodeRType(processor, stage); return; // add rd,rt,rs
  case 0x21: DecodeRType(processor, stage); return; // addu rd,rt,rs
  case 0x22: DecodeRType(processor, stage); return; // sub rd,rt,rs
  case 0x23: DecodeRType(processor, stage); return; // subu rd,rt,rs
  case 0x24: DecodeRType(processor, stage); return; // and rd,rt,rs
  case 0x25: DecodeRType(processor, stage); return; // or rd,rt,rs
  case 0x26: DecodeRType(processor, stage); return; // xor rd,rt,rs
  case 0x27: DecodeRType(processor, stage); return; // nor rd,rt,rs
//case 0x28:
//case 0x29:
  case 0x2a: DecodeRType(processor, stage); return; // slt rd,rt,rs
  case 0x2b: DecodeRType(processor, stage); return; // sltu rd,rt,rs
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

static void StageRf01(R3051* processor, R3051::Stage* stage) {
  switch (stage->rt = DECODE_RT()) {
  case 0x00: DecodeIType(processor, stage); return; // bltz rs,immediate
  case 0x01: DecodeIType(processor, stage); return; // bgez rs,immediate
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
  case 0x10: DecodeIType(processor, stage); return; // bltzal rs,immediate
  case 0x11: DecodeIType(processor, stage); return; // bgezal rs,immediate
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

void R3051::StageRf(void) {
  R3051::Stage* stage = &this->rf;

  switch (stage->op = DECODE_OP()) {
  case 0x00: StageRf00(this, stage); return;
  case 0x01: StageRf01(this, stage); return;
  case 0x02: DecodeJType(this, stage); return; // j immediate
  case 0x03: DecodeJType(this, stage); return; // jal immediate
  case 0x04: DecodeIType(this, stage); return; // beq rt,rs,immediate
  case 0x05: DecodeIType(this, stage); return; // bne rt,rs,immediate
  case 0x06: DecodeIType(this, stage); return; // blez rs,immediate
  case 0x07: DecodeIType(this, stage); return; // bgtz rs,immediate
  case 0x08: DecodeIType(this, stage); return; // addi rt,rs,immediate
  case 0x09: DecodeIType(this, stage); return; // addiu rt,rs,immediate
  case 0x0a: DecodeIType(this, stage); return; // slti rt,rs,immediate
  case 0x0b: DecodeIType(this, stage); return; // sltiu rt,rs,immediate
  case 0x0c: DecodeIType(this, stage); return; // andi rt,rs,immediate
  case 0x0d: DecodeIType(this, stage); return; // ori rt,rs,immediate
  case 0x0e: DecodeIType(this, stage); return; // xori rt,rs,immediate
  case 0x0f: DecodeIType(this, stage); return; // lui rt,immediate
  case 0x10: DecodeRType(this, stage); return; // cop0
  case 0x11: DecodeRType(this, stage); return; // cop1
  case 0x12: DecodeRType(this, stage); return; // cop2
  case 0x13: DecodeRType(this, stage); return; // cop3
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
  case 0x20: DecodeIType(this, stage); return; // lb rt,immediate(rs)
  case 0x21: DecodeIType(this, stage); return; // lh rt,immediate(rs)
  case 0x22: DecodeIType(this, stage); return; // lwl rt,immediate(rs)
  case 0x23: DecodeIType(this, stage); return; // lw rt,immediate(rs)
  case 0x24: DecodeIType(this, stage); return; // lbu rt,immediate(rs)
  case 0x25: DecodeIType(this, stage); return; // lhu rt,immediate(rs)
  case 0x26: DecodeIType(this, stage); return; // lwr rt,immediate(rs)
//case 0x27:
  case 0x28: DecodeIType(this, stage); return; // sb rt,immediate(rs)
  case 0x29: DecodeIType(this, stage); return; // sh rt,immediate(rs)
  case 0x2a: DecodeIType(this, stage); return; // swl rt,immediate(rs)
  case 0x2b: DecodeIType(this, stage); return; // sw rt,immediate(rs)
//case 0x2c:
//case 0x2d:
  case 0x2e: DecodeIType(this, stage); return; // swr rt,immediate(rs)
//case 0x2f:
  case 0x30: DecodeIType(this, stage); return; // lwc0 rt,immediate(rs)
  case 0x31: DecodeIType(this, stage); return; // lwc1 rt,immediate(rs)
  case 0x32: DecodeIType(this, stage); return; // lwc2 rt,immediate(rs)
  case 0x33: DecodeIType(this, stage); return; // lwc3 rt,immediate(rs)
//case 0x34:
//case 0x35:
//case 0x36:
//case 0x37:
  case 0x38: DecodeIType(this, stage); return; // swc0 rt,immediate(rs)
  case 0x39: DecodeIType(this, stage); return; // swc1 rt,immediate(rs)
  case 0x3a: DecodeIType(this, stage); return; // swc2 rt,immediate(rs)
  case 0x3b: DecodeIType(this, stage); return; // swc3 rt,immediate(rs)
//case 0x3c:
//case 0x3d:
//case 0x3e:
//case 0x3f:
  }

  assert(0 && "Unimplemented instruction");
}

static void DecodeIType(R3051* processor, R3051::Stage* stage) {
  stage->nn = DECODE_SI();
  stage->rt = DECODE_RT();
  stage->rs = DECODE_RS();
}

static void DecodeJType(R3051* processor, R3051::Stage* stage) {
  stage->nn = DECODE_JT();
}

static void DecodeRType(R3051* processor, R3051::Stage* stage) {
  stage->nn = DECODE_SA();
  stage->rd = DECODE_RD();
  stage->rt = DECODE_RT();
  stage->rs = DECODE_RS();
}

#undef DECODE_JT
#undef DECODE_SI
#undef DECODE_FN
#undef DECODE_SA
#undef DECODE_RD
#undef DECODE_RT
#undef DECODE_RS
#undef DECODE_OP
