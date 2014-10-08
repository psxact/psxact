#include "stdafx.h"
#include "r3051.h"
#include "cop0.h"

#define Cv stage->nn
#define Rd processor->registers[stage->rd]
#define Rs processor->registers[stage->rs]
#define Rt processor->registers[stage->rt]

#define ExLog(format, ...) //printf(format"\n", __VA_ARGS__)

#define op_impl(name) static void StageEx##name(R3051*, R3051::Stage*)
#define op_decl(name) static void StageEx##name(R3051* processor, R3051::Stage* stage)
#define op_call(name) StageEx##name(this, stage)

op_impl(add);
op_impl(addi);
op_impl(addiu);
op_impl(addu);
op_impl(and);
op_impl(andi);
op_impl(beq);
op_impl(bgez);
op_impl(bgezal);
op_impl(bgtz);
op_impl(blez);
op_impl(bltz);
op_impl(bltzal);
op_impl(bne);
op_impl(break);
op_impl(div);
op_impl(divu);
op_impl(j);
op_impl(jal);
op_impl(jalr);
op_impl(jr);
op_impl(lui);
op_impl(lb);
op_impl(lbu);
op_impl(lh);
op_impl(lhu);
op_impl(lw);
op_impl(lwc);
op_impl(lwl);
op_impl(lwr);
op_impl(mfhi);
op_impl(mflo);
op_impl(mthi);
op_impl(mtlo);
op_impl(mult);
op_impl(multu);
op_impl(nor);
op_impl(or);
op_impl(ori);
op_impl(sll);
op_impl(sllv);
op_impl(slt);
op_impl(slti);
op_impl(sltiu);
op_impl(sltu);
op_impl(sra);
op_impl(srav);
op_impl(srl);
op_impl(srlv);
op_impl(sub);
op_impl(subu);
op_impl(sb);
op_impl(sh);
op_impl(sw);
op_impl(swc);
op_impl(swl);
op_impl(swr);
op_impl(syscall);
op_impl(xor);
op_impl(xori);

void R3051::StageEx00(R3051::Stage* stage) {
  switch (stage->fn) {
  case 0x00: op_call(sll); return;
//case 0x01:
  case 0x02: op_call(srl); return;
  case 0x03: op_call(sra); return;
  case 0x04: op_call(sllv); return;
//case 0x05:
  case 0x06: op_call(srlv); return;
  case 0x07: op_call(srav); return;
  case 0x08: op_call(jr); return;
  case 0x09: op_call(jalr); return;
//case 0x0a:
//case 0x0b:
  case 0x0c: op_call(syscall); return;
  case 0x0d: op_call(break); return;
//case 0x0e:
//case 0x0f:
  case 0x10: op_call(mfhi); return;
  case 0x11: op_call(mthi); return;
  case 0x12: op_call(mflo); return;
  case 0x13: op_call(mtlo); return;
//case 0x14:
//case 0x15:
//case 0x16:
//case 0x17:
  case 0x18: op_call(mult); return;
  case 0x19: op_call(multu); return;
  case 0x1a: op_call(div); return;
  case 0x1b: op_call(divu); return;
//case 0x1c:
//case 0x1d:
//case 0x1e:
//case 0x1f:
  case 0x20: op_call(add); return;
  case 0x21: op_call(addu); return;
  case 0x22: op_call(sub); return;
  case 0x23: op_call(subu); return;
  case 0x24: op_call(and); return;
  case 0x25: op_call(or); return;
  case 0x26: op_call(xor); return;
  case 0x27: op_call(nor); return;
//case 0x28:
//case 0x29:
  case 0x2a: op_call(slt); return;
  case 0x2b: op_call(sltu); return;
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

void R3051::StageEx01(R3051::Stage* stage) {
  switch (stage->rt) {
  case 0x00: op_call(bltz); return;
  case 0x01: op_call(bgez); return;
//case 0x02:
//case 0x03:
//case 0x04:
//case 0x05:
//case 0x06:
//case 0x07:
//case 0x09:
//case 0x00:
//case 0x0a:
//case 0x0b:
//case 0x0c:
//case 0x0d:
//case 0x0e:
//case 0x0f:
  case 0x10: op_call(bltzal); return;
  case 0x11: op_call(bgezal); return;
//case 0x12:
//case 0x13:
//case 0x14:
//case 0x15:
//case 0x16:
//case 0x17:
//case 0x19:
//case 0x10:
//case 0x1a:
//case 0x1b:
//case 0x1c:
//case 0x1d:
//case 0x1e:
//case 0x1f:
  }

  assert(0 && "Unimplemented instruction");
}

void R3051::StageExCp(R3051::Stage* stage) {
  auto processor = this;

  switch (stage->op & 3) {
  case 0:
    switch (stage->rs) {
    case 0x00: Rt = cop0->FetchSr(stage->rd); return; // mfc0 rd,rt
    case 0x02: Rt = cop0->FetchCr(stage->rd); return; // cfc0 rd,rt
    case 0x04: cop0->StoreSr(stage->rd, Rt); return; // mtc0 rd,rt
    case 0x06: cop0->StoreCr(stage->rd, Rt); return; // ctc0 rd,rt
    case 0x10: // cop0
      if (cop0->Execute(stage->code & 0x1f)) {
        return;
      }
      break;
    }
    break;
  }

  assert(0 && "Unimplemented coprocessor instruction");
}

void R3051::StageEx(void) {
  R3051::Stage* stage = &this->ex;

  switch (stage->op) {
  case 0x00: this->StageEx00(stage); return;
  case 0x01: this->StageEx01(stage); return;
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
  case 0x10: this->StageExCp(stage); return;
  case 0x11: this->StageExCp(stage); return;
  case 0x12: this->StageExCp(stage); return;
  case 0x13: this->StageExCp(stage); return;
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
  case 0x21: op_call(lh); return;
  case 0x22: op_call(lwl); return;
  case 0x23: op_call(lw); return;
  case 0x24: op_call(lbu); return;
  case 0x25: op_call(lhu); return;
  case 0x26: op_call(lwr); return;
//case 0x27:
  case 0x28: op_call(sb); return;
  case 0x29: op_call(sh); return;
  case 0x2a: op_call(swl); return;
  case 0x2b: op_call(sw); return;
//case 0x2c:
//case 0x2d:
  case 0x2e: op_call(swr); return;
//case 0x2f:
  case 0x30: op_call(lwc); return;
  case 0x31: op_call(lwc); return;
  case 0x32: op_call(lwc); return;
  case 0x33: op_call(lwc); return;
//case 0x34:
//case 0x35:
//case 0x36:
//case 0x37:
  case 0x38: op_call(swc); return;
  case 0x39: op_call(swc); return;
  case 0x3a: op_call(swc); return;
  case 0x3b: op_call(swc); return;
//case 0x3c:
//case 0x3d:
//case 0x3e:
//case 0x3f:
  }

  assert(0 && "Unimplemented instruction");
}

op_decl(add) {
  uint32_t sum = Rs + Rt;

  // todo: integer overflow exception

  Rd = sum;
}

op_decl(addi) {
  uint32_t sum = Rs + Util::Sign<16>(Cv);

  // todo: integer overflow exception

  Rt = sum;
}

op_decl(addiu) {
  Rt = Rs + Util::Sign<16>(Cv);
}

op_decl(addu) {
  Rd = Rs + Rt;
}

op_decl(and) {
  Rd = Rs & Rt;
}

op_decl(andi) {
  Rt = Rs & Cv;
}

op_decl(beq) {
  if (Rt == Rs) {
    processor->pc -= 4;
    processor->pc += Util::Sign<16>(Cv) << 2;
  }
}

op_decl(bgez) {
  if (((int32_t)Rs) >= 0) {
    processor->pc -= 4;
    processor->pc += Util::Sign<16>(Cv) << 2;
  }
}

op_decl(bgezal) {
  assert(0 && "unimplemented instruction: BGEZAL");
}

op_decl(bgtz) {
  if (((int32_t) Rs) > 0) {
    processor->pc -= 4;
    processor->pc += Util::Sign<16>(Cv) << 2;
  }
}

op_decl(blez) {
  if (((int32_t)Rs) <= 0) {
    processor->pc -= 4;
    processor->pc += Util::Sign<16>(Cv) << 2;
  }
}

op_decl(bltz) {
  if (((int32_t)Rs) < 0) {
    processor->pc -= 4;
    processor->pc += Util::Sign<16>(Cv) << 2;
  }
}

op_decl(bltzal) {
  assert(0 && "unimplemented instruction: BLTZAL");
}

op_decl(bne) {
  if (Rt != Rs) {
    processor->pc -= 4;
    processor->pc += Util::Sign<16>(Cv) << 2;
  }
}

op_decl(break) {
  assert(0 && "unimplemented instruction: BREAK");
}

op_decl(div) {
  int32_t rs = (int32_t) Rs;
  int32_t rt = (int32_t) Rt;

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
  uint32_t rs = Rs;
  uint32_t rt = Rt;

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
  processor->pc = ((processor->pc - 4) & 0xf0000000) | (Cv << 2);

  ExLog("[$%08x] j $%08x", stage->address, processor->pc);
}

op_decl(jal) {
  processor->registers[31] = processor->pc;
  processor->pc = ((processor->pc - 4) & 0xf0000000) | (Cv << 2);

  ExLog("[$%08x] jal $%08x", stage->address, processor->pc);
}

op_decl(jalr) {
  Rd = processor->pc;
  processor->pc = Rs;

  ExLog("[$%08x] jalr r%d,r%d", stage->address, stage->rd, stage->rs);
}

op_decl(jr) {
  processor->pc = Rs;

  ExLog("[$%08x] jr r%d", stage->address, stage->rs);
}

op_decl(lui) {
  Rt = Cv << 16;
}

op_decl(lb) {
  stage->target = Rs + Util::Sign<16>(Cv);
}

op_decl(lbu) {
  stage->target = Rs + Util::Sign<16>(Cv);
}

op_decl(lh) {
  stage->target = Rs + Util::Sign<16>(Cv);
}

op_decl(lhu) {
  stage->target = Rs + Util::Sign<16>(Cv);
}

op_decl(lw) {
  stage->target = Rs + Util::Sign<16>(Cv);
}

op_decl(lwc) {
  assert(0 && "unimplemented instruction: LWC");
}

op_decl(lwl) {
  assert(0 && "unimplemented instruction: LWL");
}

op_decl(lwr) {
  assert(0 && "unimplemented instruction: LWR");
}

op_decl(mfhi) {
  Rd = processor->hi;
}

op_decl(mflo) {
  Rd = processor->lo;
}

op_decl(mthi) {
  processor->hi = Rs;
}

op_decl(mtlo) {
  processor->lo = Rs;
}

op_decl(mult) {
  assert(0 && "unimplemented instruction: MULT");
}

op_decl(multu) {
  uint64_t result = ((uint64_t) Rt) * ((uint64_t) Rs);

  processor->lo = (uint32_t) (result >> 0);
  processor->hi = (uint32_t) (result >> 32);
}

op_decl(nor) {
  Rd = ~(Rs | Rt);
}

op_decl(or) {
  Rd = Rs | Rt;
}

op_decl(ori) {
  Rt = Rs | Cv;
}

op_decl(sll) {
  Rd = Rt << Cv;
}

op_decl(sllv) {
  Rd = Rt << Rs;
}

op_decl(slt) {
  Rd =
    ((int32_t) Rs) < ((int32_t) Rt);
}

op_decl(slti) {
  Rt = 
    ((int32_t) Rs) < ((int16_t) Cv);
}

op_decl(sltiu) {
  Rt = Rs < ((uint32_t) ((int16_t) Cv));
}

op_decl(sltu) {
  Rd = Rs < Rt;
}

op_decl(sra) {
  Rd = ((int32_t) Rt) >> Cv;
}

op_decl(srav) {
  Rd = ((int32_t) Rt) >> Rs;
}

op_decl(srl) {
  Rd = Rt >> Cv;
}

op_decl(srlv) {
  Rd = Rt >> Rs;
}

op_decl(sub) {
  uint32_t difference = Rs - Rt;

  // todo: arithmetic overflow exception

  Rd = difference;
}

op_decl(subu) {
  Rd = Rs - Rt;
}

op_decl(sb) {
  stage->target = Rs + Util::Sign<16>(Cv);
}

op_decl(sh) {
  stage->target = Rs + Util::Sign<16>(Cv);
}

op_decl(sw) {
  stage->target = Rs + Util::Sign<16>(Cv);
}

op_decl(swc) {
  assert(0 && "unimplemented instruction: SWC");
}

op_decl(swl) {
  assert(0 && "unimplemented instruction: SWL");
}

op_decl(swr) {
  assert(0 && "unimplemented instruction: SWR");
}

op_decl(syscall) {
  processor->cop0->SysCall(processor);
}

op_decl(xor) {
  Rd = Rs ^ Rt;
}

op_decl(xori) {
  Rt = Rs ^ Cv;
}
