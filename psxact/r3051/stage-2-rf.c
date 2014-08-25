#include "stdafx.h"
#include "r3051.h"

#define DECODE_SI() (stage->code >>  0) & 0xffff
#define DECODE_FN() (stage->code >>  0) & 63
#define DECODE_SA() (stage->code >>  6) & 31
#define DECODE_RD() (stage->code >> 11) & 31
#define DECODE_RT() (stage->code >> 16) & 31
#define DECODE_RS() (stage->code >> 21) & 31
#define DECODE_OP() (stage->code >> 26) & 63

void escape_table_00(struct r3051_pipestage*);
void escape_table_01(struct r3051_pipestage*);
void escape_table_cp(struct r3051_pipestage*, int);

STAGE(rf) {
  struct r3051_pipestage *stage = &processor->rf;

  switch (DECODE_OP()) {
  case 0x00: escape_table_00(stage); break;
  case 0x01: escape_table_01(stage); break;
  case 0x02: // j   long immediate
  case 0x03: // jal long immediate
    break;

  case 0x04: // beq   rt,rs,immediate
  case 0x05: // bne   rt,rs,immediate
  case 0x06: // blez     rs,immediate
  case 0x07: // bgtz     rs,immediate
  case 0x08: // addi  rt,rs,immediate
  case 0x09: // addiu rt,rs,immediate
  case 0x0a: // slti  rt,rs,immediate
  case 0x0b: // sltiu rt,rs,immediate
  case 0x0c: // andi  rt,rs,immediate
  case 0x0d: // ori   rt,rs,immediate
  case 0x0e: // xori  rt,rs,immediate
  case 0x0f: // lui   rt,   immediate
    DECODE_SI();
    DECODE_RT();
    DECODE_RS();
    break;
    
  case 0x10: escape_table_cp(stage, 0); break; // cop0  rt,rd
  case 0x11: escape_table_cp(stage, 1); break; // cop1  rt,rd
  case 0x12: escape_table_cp(stage, 2); break; // cop2  rt,rd
  case 0x13: escape_table_cp(stage, 3); break; // cop3  rt,rd
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
  case 0x20: // lb    rt,rs,immediate
  case 0x21: // lh    rt,rs,immediate
  case 0x22: // lwl   rt,rs,immediate
  case 0x23: // lw    rt,rs,immediate
  case 0x24: // lbu   rt,rs,immediate
  case 0x25: // lhu   rt,rs,immediate
  case 0x26: // lwr   rt,rs,immediate
//case 0x27:
  case 0x28: // sb    rt,rs,immediate
  case 0x29: // sh    rt,rs,immediate
  case 0x2a: // swl   rt,rs,immediate
  case 0x2b: // sw    rt,rs,immediate
//case 0x2c:
//case 0x2d:
  case 0x2e: // swr   rt,rs,immediate
//case 0x2f:
  case 0x30: // lwc0  rt,rs,immediate
  case 0x31: // lwc1  rt,rs,immediate
  case 0x32: // lwc2  rt,rs,immediate
  case 0x33: // lwc3  rt,rs,immediate
//case 0x34:
//case 0x35:
//case 0x36:
//case 0x37:
  case 0x38: // swc0  rt,rs,immediate
  case 0x39: // swc1  rt,rs,immediate
  case 0x3a: // swc2  rt,rs,immediate
  case 0x3b: // swc3  rt,rs,immediate
//case 0x3c:
//case 0x3d:
//case 0x3e:
//case 0x3f:
    DECODE_SI();
    DECODE_RT();
    DECODE_RS();
    break;
  }

  processor->rf = processor->ic;
}

void escape_table_00(struct r3051_pipestage* stage) {
  switch (DECODE_FN()) {
  case 0x00: // sll   rd,   rt,sa
//case 0x01:
  case 0x02: // srl   rd,   rt,sa
  case 0x03: // sra   rd,   rt,sa
    DECODE_SA();
    DECODE_RD();
    DECODE_RT();
    break;

  case 0x04: // sllv  rd,rs,rt
//case 0x05:
  case 0x06: // srlv  rd,rs,rt
  case 0x07: // srav  rd,rs,rt
    DECODE_RD();
    DECODE_RT();
    DECODE_RS();
    break;

  case 0x08: // jr       rs
  case 0x09: // jalr  rd,rs
//case 0x0a:
//case 0x0b:
    break;

  case 0x0c: // syscall
  case 0x0d: // break
    break;

//case 0x0e:
//case 0x0f:
  case 0x10: // mfhi  rd
  case 0x11: // mthi     rs
  case 0x12: // mflo  rd
  case 0x13: // mtlo     rs
//case 0x14:
//case 0x15:
//case 0x16:
  case 0x17: // nor   rd,rs,rt
  case 0x18: // mult     rs,rt
  case 0x19: // multu    rs,rt
  case 0x1a: // div      rs,rt
  case 0x1b: // divu     rs,rt
//case 0x1c:
//case 0x1d:
//case 0x1e:
//case 0x1f:
  case 0x20: // add   rd,rs,rt
  case 0x21: // addu  rd,rs,rt
  case 0x22: // sub   rd,rs,rt
  case 0x23: // subu  rd,rs,rt
  case 0x24: // and   rd,rs,rt
  case 0x25: // or    rd,rs,rt
  case 0x26: // xor   rd,rs,rt
//case 0x27:
//case 0x28:
//case 0x29:
  case 0x2a: // slt   rd,rs,rt
  case 0x2b: // sltu  rd,rs,rt
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
    DECODE_RD();
    DECODE_RT();
    DECODE_RS();
    break;
  }
}

void escape_table_01(struct r3051_pipestage* stage) {
  switch (DECODE_RT()) { // 000001 ----- xxxxx ----------------
  case 0x00: // bltz   rs,immediate
  case 0x01: // bgez   rs,immediate
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
  case 0x10: // bltzal rs,immediate
  case 0x11: // bgezal rs,immediate
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
    DECODE_SI();
    DECODE_RS();
    break;
  }
}

void escape_table_cp(struct r3051_pipestage* stage, int coprocessor) {
  if (stage->code & (1 << 25)) {
    // coprocessor operation
    return;
  }

  switch (DECODE_RS()) {
  case 0x00: break; // mfc rt,rd
//case 0x01:
  case 0x02: break; // cfc rt,rd
//case 0x03:
  case 0x04: break; // mtc rt,rd
//case 0x05:
  case 0x06: break; // ctc rt,rd
//case 0x07:
//case 0x08:
//case 0x09:
//case 0x0a:
//case 0x0b:
//case 0x0c:
//case 0x0d:
//case 0x0e:
//case 0x0f:
//case 0x10:
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
  }
}
