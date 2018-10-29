// Copyright 2018 psxact

#include "cpu/core.hpp"

#include <string>


using psx::cpu::core_t;

#define get_iconst() decode_iconst()
#define get_uconst() decode_uconst()
#define get_rd() register_names[decode_rd()]
#define get_rs() register_names[decode_rs()]
#define get_rt() register_names[decode_rt()]
#define get_sa() decode_sa()


static const char *register_names[32] = {
  "r0",
  "at",
  "v0", "v1",
  "a0", "a1", "a2", "a3",
  "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
  "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
  "t8", "t9",
  "k0", "k1",
  "gp",
  "sp",
  "fp",
  "ra"
};


void core_t::disassemble_special(FILE *file) {
  switch (code & 0x3f) {
  case 0x00: fprintf(file, "sll       %s, %s, #%d\n", get_rd(), get_rt(), get_sa()); break;

  case 0x02: fprintf(file, "srl       %s, %s, #%d\n", get_rd(), get_rt(), get_sa()); break;
  case 0x03: fprintf(file, "sra       %s, %s, #%d\n", get_rd(), get_rt(), get_sa()); break;
  case 0x04: fprintf(file, "sllv      %s, %s, %s\n", get_rd(), get_rt(), get_rs()); break;

  case 0x06: fprintf(file, "srlv      %s, %s, %s\n", get_rd(), get_rt(), get_rs()); break;
  case 0x07: fprintf(file, "srav      %s, %s, %s\n", get_rd(), get_rt(), get_rs()); break;
  case 0x08: fprintf(file, "jr        %s\n", get_rs()); break;
  case 0x09: fprintf(file, "jalr      %s, %s\n", get_rd(), get_rs()); break;

  case 0x0c: fprintf(file, "syscall   \n"); break;
  case 0x0d: fprintf(file, "break     \n"); break;

  case 0x10: fprintf(file, "mfhi      %s\n", get_rs()); break;
  case 0x11: fprintf(file, "mthi      %s\n", get_rs()); break;
  case 0x12: fprintf(file, "mflo      %s\n", get_rs()); break;
  case 0x13: fprintf(file, "mtlo      %s\n", get_rs()); break;

  case 0x18: fprintf(file, "mult      %s, %s\n", get_rs(), get_rt()); break;
  case 0x19: fprintf(file, "multu     %s, %s\n", get_rs(), get_rt()); break;
  case 0x1a: fprintf(file, "div       %s, %s\n", get_rs(), get_rt()); break;
  case 0x1b: fprintf(file, "divu      %s, %s\n", get_rs(), get_rt()); break;

  case 0x20: fprintf(file, "add       %s, %s, %s\n", get_rd(), get_rs(), get_rt()); break;
  case 0x21: fprintf(file, "addu      %s, %s, %s\n", get_rd(), get_rs(), get_rt()); break;
  case 0x22: fprintf(file, "sub       %s, %s, %s\n", get_rd(), get_rs(), get_rt()); break;
  case 0x23: fprintf(file, "subu      %s, %s, %s\n", get_rd(), get_rs(), get_rt()); break;
  case 0x24: fprintf(file, "and       %s, %s, %s\n", get_rd(), get_rs(), get_rt()); break;
  case 0x25: fprintf(file, "or        %s, %s, %s\n", get_rd(), get_rs(), get_rt()); break;
  case 0x26: fprintf(file, "xor       %s, %s, %s\n", get_rd(), get_rs(), get_rt()); break;
  case 0x27: fprintf(file, "nor       %s, %s, %s\n", get_rd(), get_rs(), get_rt()); break;

  case 0x2a: fprintf(file, "slt       %s, %s, %s\n", get_rd(), get_rs(), get_rt()); break;
  case 0x2b: fprintf(file, "sltu      %s, %s, %s\n", get_rd(), get_rs(), get_rt()); break;

  default:
    fprintf(file, "unknown (0x%08x)\n", code);
    break;
  }
}


void core_t::disassemble_reg_imm(FILE *file) {
  uint32_t pc = regs.this_pc;

  switch (decode_rt()) {
  case 0x00: fprintf(file, "bltz      %s, 0x%08x\n", get_rs(), pc + 4 + (get_iconst() << 2)); break;
  case 0x01: fprintf(file, "bgez      %s, 0x%08x\n", get_rs(), pc + 4 + (get_iconst() << 2)); break;

  case 0x10: fprintf(file, "bltzal    %s, 0x%08x\n", get_rs(), pc + 4 + (get_iconst() << 2)); break;
  case 0x11: fprintf(file, "bgezal    %s, 0x%08x\n", get_rs(), pc + 4 + (get_iconst() << 2)); break;

  default:
    fprintf(file, "unknown (0x%08x)\n", code);
    break;
  }
}


void core_t::disassemble(FILE *file) {
  uint32_t pc = regs.this_pc;

  fprintf(file, "%08X: ", pc);

  if (code == 0) {
    fprintf(file, "nop       \n");
    return;
  }

  switch ((code >> 26) & 0x3f) {
  case 0x00: disassemble_special(file); break;
  case 0x01: disassemble_reg_imm(file); break;

  case 0x02: fprintf(file, "j         0x%08x\n", (pc & ~0x0fffffff) | ((code << 2) & 0x0fffffff)); break;
  case 0x03: fprintf(file, "jal       0x%08x\n", (pc & ~0x0fffffff) | ((code << 2) & 0x0fffffff)); break;

  case 0x04: fprintf(file, "beq       %s, %s, 0x%08x\n", get_rs(), get_rt(), pc + 4 + (get_iconst() << 2)); break;
  case 0x05: fprintf(file, "bne       %s, %s, 0x%08x\n", get_rs(), get_rt(), pc + 4 + (get_iconst() << 2)); break;
  case 0x06: fprintf(file, "blez      %s, 0x%08x\n", get_rs(), pc + 4 + (get_iconst() << 2)); break;
  case 0x07: fprintf(file, "bgtz      %s, 0x%08x\n", get_rs(), pc + 4 + (get_iconst() << 2)); break;

  case 0x08: fprintf(file, "addi      %s, %s, 0x%04x\n", get_rt(), get_rs(), get_iconst()); break;
  case 0x09: fprintf(file, "addiu     %s, %s, 0x%04x\n", get_rt(), get_rs(), get_iconst()); break;
  case 0x0a: fprintf(file, "slti      %s, %s, 0x%04x\n", get_rt(), get_rs(), get_iconst()); break;
  case 0x0b: fprintf(file, "sltiu     %s, %s, 0x%04x\n", get_rt(), get_rs(), get_iconst()); break;
  case 0x0c: fprintf(file, "andi      %s, %s, 0x%04x\n", get_rt(), get_rs(), get_uconst()); break;
  case 0x0d: fprintf(file, "ori       %s, %s, 0x%04x\n", get_rt(), get_rs(), get_uconst()); break;
  case 0x0e: fprintf(file, "xori      %s, %s, 0x%04x\n", get_rt(), get_rs(), get_uconst()); break;
  case 0x0f: fprintf(file, "lui       %s, 0x%04x\n", get_rt(), get_uconst()); break;

  case 0x10:
  case 0x11:
  case 0x12:
  case 0x13: {
    uint32_t co = (code >> 26) & 3;

    switch (decode_rs()) {
    case 0x00: fprintf(file, "mfc%d      %s, %s\n", co, get_rt(), get_rd()); break;
    case 0x02: fprintf(file, "cfc%d      %s, %s\n", co, get_rt(), get_rd()); break;
    case 0x04: fprintf(file, "mtc%d      %s, %s\n", co, get_rt(), get_rd()); break;
    case 0x06: fprintf(file, "ctc%d      %s, %s\n", co, get_rt(), get_rd()); break;

    case 0x10:
      switch (code & 0x3f) {
      case 0x10: fprintf(file, "rfe\n"); break;

      default:
        fprintf(file, "unknown (0x%08x)\n", code);
        break;
      }
      break;

    default:
      fprintf(file, "unknown (0x%08x)\n", code);
      break;
    }

    break;
  }

  case 0x20: fprintf(file, "lb        %s, 0x%04x(%s)\n", get_rt(), get_iconst(), get_rs()); break;
  case 0x21: fprintf(file, "lh        %s, 0x%04x(%s)\n", get_rt(), get_iconst(), get_rs()); break;
  case 0x22: fprintf(file, "lwl       %s, 0x%04x(%s)\n", get_rt(), get_iconst(), get_rs()); break;
  case 0x23: fprintf(file, "lw        %s, 0x%04x(%s)\n", get_rt(), get_iconst(), get_rs()); break;
  case 0x24: fprintf(file, "lbu       %s, 0x%04x(%s)\n", get_rt(), get_iconst(), get_rs()); break;
  case 0x25: fprintf(file, "lhu       %s, 0x%04x(%s)\n", get_rt(), get_iconst(), get_rs()); break;
  case 0x26: fprintf(file, "lwr       %s, 0x%04x(%s)\n", get_rt(), get_iconst(), get_rs()); break;

  case 0x28: fprintf(file, "sb        %s, 0x%04x(%s)\n", get_rt(), get_iconst(), get_rs()); break;
  case 0x29: fprintf(file, "sh        %s, 0x%04x(%s)\n", get_rt(), get_iconst(), get_rs()); break;
  case 0x2a: fprintf(file, "swl       %s, 0x%04x(%s)\n", get_rt(), get_iconst(), get_rs()); break;
  case 0x2b: fprintf(file, "sw        %s, 0x%04x(%s)\n", get_rt(), get_iconst(), get_rs()); break;

  case 0x2e: fprintf(file, "swr       %s, 0x%04x(%s)\n", get_rt(), get_iconst(), get_rs()); break;

  default:
    fprintf(file, "unknown (0x%08x)\n", code);
    break;
  }
}
