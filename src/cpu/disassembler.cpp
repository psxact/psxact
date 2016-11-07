#include <string>
#include <stdexcept>
#include "cpu_core.hpp"
#include "../utility.hpp"

static uint32_t iconst() {
  return utility::sclip<16>(cpu::state.code);
}

static uint32_t uconst() {
  return utility::uclip<16>(cpu::state.code);
}

static uint32_t sa() {
  return (cpu::state.code >>  6) & 0x1f;
}

static uint32_t rd() {
  return (cpu::state.code >> 11) & 0x1f;
}

static uint32_t rt() {
  return (cpu::state.code >> 16) & 0x1f;
}

static uint32_t rs() {
  return (cpu::state.code >> 21) & 0x1f;
}

void disassemble_special(uint32_t pc) {
  switch ((cpu::state.code >> 0) & 0x3f) {
    case 0x00: /*printf("sll    r%02d, r%02d, #%d\n", rd(), rt(), sa());*/ break;

    case 0x02: /*printf("srl    r%02d, r%02d, #%d\n", rd(), rt(), sa());*/ break;
    case 0x03: /*printf("sra    r%02d, r%02d, #%d\n", rd(), rt(), sa());*/ break;
    case 0x04: /*printf("sllv   r%02d, r%02d, r%02d\n", rd(), rt(), rs());*/ break;

    case 0x06: /*printf("srlv   r%02d, r%02d, r%02d\n", rd(), rt(), rs());*/ break;
    case 0x07: /*printf("srav   r%02d, r%02d, r%02d\n", rd(), rt(), rs());*/ break;
    case 0x08: /*printf("jr     r%02d\n", rs());*/ break;
    case 0x09: /*printf("jalr   r%02d, r%02d\n", rd(), rs());*/ break;

    case 0x0c: /*printf("syscall\n");*/ break;
    case 0x0d: /*printf("break\n");*/ break;

    case 0x10: /*printf("mfhi   r%02d\n", rs());*/ break;
    case 0x11: /*printf("mthi   r%02d\n", rs());*/ break;
    case 0x12: /*printf("mflo   r%02d\n", rd());*/ break;
    case 0x13: /*printf("mtlo   r%02d\n", rs());*/ break;

    case 0x18: /*printf("mult   r%02d, r%02d\n", rs(), rt());*/ break;
    case 0x19: /*printf("multu  r%02d, r%02d\n", rs(), rt());*/ break;
    case 0x1a: /*printf("div    r%02d, r%02d\n", rs(), rt());*/ break;
    case 0x1b: /*printf("divu   r%02d, r%02d\n", rs(), rt());*/ break;

    case 0x20: /*printf("add    r%02d, r%02d, r%02d\n", rd(), rs(), rt());*/ break;
    case 0x21: /*printf("addu   r%02d, r%02d, r%02d\n", rd(), rs(), rt());*/ break;
    case 0x22: /*printf("sub    r%02d, r%02d, r%02d\n", rd(), rs(), rt());*/ break;
    case 0x23: /*printf("subu   r%02d, r%02d, r%02d\n", rd(), rs(), rt());*/ break;
    case 0x24: /*printf("and    r%02d, r%02d, r%02d\n", rd(), rs(), rt());*/ break;
    case 0x25: /*printf("or     r%02d, r%02d, r%02d\n", rd(), rs(), rt());*/ break;
    case 0x26: /*printf("xor    r%02d, r%02d, r%02d\n", rd(), rs(), rt());*/ break;
    case 0x27: /*printf("nor    r%02d, r%02d, r%02d\n", rd(), rs(), rt());*/ break;

    case 0x2a: /*printf("slt    r%02d, r%02d, r%02d\n", rd(), rs(), rt());*/ break;
    case 0x2b: /*printf("sltu   r%02d, r%02d, r%02d\n", rd(), rs(), rt());*/ break;

    default:
      printf("0x%08x | unknown (0x%08x)\n", pc, cpu::state.code);
      break;
  }
}

void disassemble_reg_imm(uint32_t pc) {
  switch (rt()) {
    case 0x00: /*printf("bltz   r%02d, #0x%08x\n", rs(), pc + 4 + (iconst() << 2));*/ break;
    case 0x01: /*printf("bgez   r%02d, #0x%08x\n", rs(), pc + 4 + (iconst() << 2));*/ break;

    case 0x10: /*printf("bltzal r%02d, #0x%08x\n", rs(), pc + 4 + (iconst() << 2));*/ break;
    case 0x11: /*printf("bgezal r%02d, #0x%08x\n", rs(), pc + 4 + (iconst() << 2));*/ break;

    default:
      printf("0x%08x | unknown (0x%08x)\n", pc, cpu::state.code);
      break;
  }
}

void cpu::disassemble() {
  auto pc = state.regs.this_pc;

  switch ((cpu::state.code >> 26) & 0x3f) {
    case 0x00: return disassemble_special(pc);
    case 0x01: return disassemble_reg_imm(pc);

    case 0x02: /*printf("j      0x%08x\n", (pc & ~0x0fffffff) | ((cpu::state.code << 2) & 0x0fffffff));*/ break;
    case 0x03: /*printf("jal    0x%08x\n", (pc & ~0x0fffffff) | ((cpu::state.code << 2) & 0x0fffffff));*/ break;

    case 0x04: /*printf("beq    r%02d, r%02d, #0x%08x\n", rs(), rt(), pc + 4 + (iconst() << 2));*/ break;
    case 0x05: /*printf("bne    r%02d, r%02d, #0x%08x\n", rs(), rt(), pc + 4 + (iconst() << 2));*/ break;
    case 0x06: /*printf("blez   r%02d, #0x%08x\n", rs(), pc + 4 + (iconst() << 2));*/ break;
    case 0x07: /*printf("bgtz   r%02d, #0x%08x\n", rs(), pc + 4 + (iconst() << 2));*/ break;

    case 0x08: /*printf("addi   r%02d, r%02d, #0x%08x\n", rt(), rs(), iconst());*/ break;
    case 0x09: /*printf("addiu  r%02d, r%02d, #0x%08x\n", rt(), rs(), iconst());*/ break;
    case 0x0a: /*printf("slti   r%02d, r%02d, #0x%08x\n", rt(), rs(), iconst());*/ break;
    case 0x0b: /*printf("sltiu  r%02d, r%02d, #0x%08x\n", rt(), rs(), iconst());*/ break;
    case 0x0c: /*printf("andi   r%02d, r%02d, #0x%04x\n", rt(), rs(), uconst());*/ break;
    case 0x0d: /*printf("ori    r%02d, r%02d, #0x%04x\n", rt(), rs(), uconst());*/ break;
    case 0x0e: /*printf("xori   r%02d, r%02d, #0x%04x\n", rt(), rs(), uconst());*/ break;
    case 0x0f: /*printf("lui    r%02d, #0x%04x\n", rt(), uconst());*/ break;

    case 0x10:
    case 0x11:
    case 0x12:
    case 0x13: {
      /*auto co = (cpu::state.code >> 26) & 3;*/

      switch (rs()) {
        case 0x00: /*printf("mfc%d   r%02d, r%02d\n", co, rt(), rd());*/ break;
        case 0x04: /*printf("mtc%d   r%02d, r%02d\n", co, rt(), rd());*/ break;

        case 0x10:
          switch ((cpu::state.code >> 0) & 0x3f) {
            case 0x10: /*printf("rfe\n");*/ break;

            default:
              printf("0x%08x | unknown (0x%08x)\n", pc, cpu::state.code);
              break;
          }
          break;

        default:
          printf("0x%08x | unknown (0x%08x)\n", pc, cpu::state.code);
          break;
      }

      break;
    }

    case 0x20: /*printf("lb     r%02d, #%08x(r%02d)\n", rt(), iconst(), rs());*/ break;
    case 0x21: /*printf("lh     r%02d, #%08x(r%02d)\n", rt(), iconst(), rs());*/ break;
    case 0x22: /*printf("lwl    r%02d, #%08x(r%02d)\n", rt(), iconst(), rs());*/ break;
    case 0x23: /*printf("lw     r%02d, #%08x(r%02d)\n", rt(), iconst(), rs());*/ break;
    case 0x24: /*printf("lbu    r%02d, #%08x(r%02d)\n", rt(), iconst(), rs());*/ break;
    case 0x25: /*printf("lhu    r%02d, #%08x(r%02d)\n", rt(), iconst(), rs());*/ break;
    case 0x26: /*printf("lwr    r%02d, #%08x(r%02d)\n", rt(), iconst(), rs());*/ break;

    case 0x28: /*printf("sb     r%02d, #%08x(r%02d)\n", rt(), iconst(), rs());*/ break;
    case 0x29: /*printf("sh     r%02d, #%08x(r%02d)\n", rt(), iconst(), rs());*/ break;
    case 0x2a: /*printf("swl    r%02d, #%08x(r%02d)\n", rt(), iconst(), rs());*/ break;
    case 0x2b: /*printf("sw     r%02d, #%08x(r%02d)\n", rt(), iconst(), rs());*/ break;

    case 0x2e: /*printf("swr    r%02d, #%08x(r%02d)\n", rt(), iconst(), rs());*/ break;

    default:
      printf("0x%08x | unknown (0x%08x)\n", pc, cpu::state.code);
      break;
  }
}
