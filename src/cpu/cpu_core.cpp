#include "cpu_core.hpp"
#include "decoder.hpp"
#include "../bus.hpp"
#include "../utility.hpp"

cpu::state_t cpu::state;

void cpu::initialize() {
  state.registers.u[0] = 0;
  state.registers.pc = 0xbfc00000;
  state.registers.next_pc = state.registers.pc + 4;
}

void cpu::main() {
  while (1) {
    decoder::code = cpu::read_code();

    state.is_branch_delay_slot = state.is_branch;
    state.is_branch = false;

    switch (decoder::op_hi()) {
    case 0x00: // special
      switch (decoder::op_lo()) {
      case 0x00: // sll rd,rt,sa
        state.registers.u[decoder::rd()] = state.registers.u[decoder::rt()] << decoder::sa();
        continue;

      case 0x02: // srl rd,rt,sa
        state.registers.u[decoder::rd()] = state.registers.u[decoder::rt()] >> decoder::sa();
        continue;

      case 0x03: // sra rd,rt,sa
        state.registers.i[decoder::rd()] = state.registers.i[decoder::rt()] >> decoder::sa();
        continue;

      case 0x04: // sllv rd,rt,rs
        state.registers.u[decoder::rd()] = state.registers.u[decoder::rt()] << state.registers.u[decoder::rs()];
        continue;

      case 0x06: // srlv rd,rt,rs
        state.registers.u[decoder::rd()] = state.registers.u[decoder::rt()] >> state.registers.u[decoder::rs()];
        continue;

      case 0x07: // srav rd,rt,rs
        state.registers.i[decoder::rd()] = state.registers.i[decoder::rt()] >> state.registers.u[decoder::rs()];
        continue;

      case 0x08: // jr rs
        state.registers.next_pc = state.registers.u[decoder::rs()];
        continue;

      case 0x09: // jalr rd,rs
        state.registers.u[decoder::rd()] = state.registers.next_pc;
        state.registers.next_pc = state.registers.u[decoder::rs()];
        continue;

      case 0x0c: // syscall
        enter_exception(0x08, state.registers.pc - 4);
        continue;

      case 0x0d: // break
        enter_exception(0x09, state.registers.pc - 4);
        continue;

      case 0x10: // mfhi rd
        state.registers.u[decoder::rd()] = state.registers.hi;
        continue;

      case 0x11: // mthi rs
        state.registers.hi = state.registers.u[decoder::rs()];
        continue;

      case 0x12: // mflo rd
        state.registers.u[decoder::rd()] = state.registers.lo;
        continue;

      case 0x13: // mtlo rs
        state.registers.lo = state.registers.u[decoder::rs()];
        continue;

      case 0x18: // mult rs,rt
      {
        int64_t result = int64_t(state.registers.i[decoder::rs()]) * int64_t(state.registers.i[decoder::rt()]);
        state.registers.lo = uint32_t(result >> 0);
        state.registers.hi = uint32_t(result >> 32);
        continue;
      }

      case 0x19: // multu rs,rt
      {
        uint64_t result = uint64_t(state.registers.u[decoder::rs()]) * uint64_t(state.registers.u[decoder::rt()]);
        state.registers.lo = uint32_t(result >> 0);
        state.registers.hi = uint32_t(result >> 32);
        continue;
      }

      case 0x1a: // div rs,rt
        if (state.registers.u[decoder::rt()]) {
          state.registers.lo = uint32_t(state.registers.i[decoder::rs()] / state.registers.i[decoder::rt()]);
          state.registers.hi = uint32_t(state.registers.i[decoder::rs()] % state.registers.i[decoder::rt()]);
        }
        else {
          state.registers.lo = ((state.registers.u[decoder::rs()] >> 30) & 2) - 1;
          state.registers.hi = state.registers.u[decoder::rs()];
        }
        continue;

      case 0x1b: // divu rs,rt
        if (state.registers.u[decoder::rt()]) {
          state.registers.lo = state.registers.u[decoder::rs()] / state.registers.u[decoder::rt()];
          state.registers.hi = state.registers.u[decoder::rs()] % state.registers.u[decoder::rt()];
        }
        else {
          state.registers.lo = 0xffffffff;
          state.registers.hi = state.registers.u[decoder::rs()];
        }
        continue;

      case 0x20: // add rd,rs,rt
        state.registers.u[decoder::rd()] = state.registers.u[decoder::rs()] + state.registers.u[decoder::rt()];
        // todo: overflow exception
        continue;

      case 0x21: // addu rd,rs,rt
        state.registers.u[decoder::rd()] = state.registers.u[decoder::rs()] + state.registers.u[decoder::rt()];
        continue;

      case 0x22: // sub rd,rs,rt
        state.registers.u[decoder::rd()] = state.registers.u[decoder::rs()] - state.registers.u[decoder::rt()];
        // todo: overflow exception
        continue;

      case 0x23: // subu rd,rs,rt
        state.registers.u[decoder::rd()] = state.registers.u[decoder::rs()] - state.registers.u[decoder::rt()];
        continue;

      case 0x24: // and rd,rs,rt
        state.registers.u[decoder::rd()] = state.registers.u[decoder::rs()] & state.registers.u[decoder::rt()];
        continue;

      case 0x25: // or rd,rs,rt
        state.registers.u[decoder::rd()] = state.registers.u[decoder::rs()] | state.registers.u[decoder::rt()];
        continue;

      case 0x26: // xor rd,rs,rt
        state.registers.u[decoder::rd()] = state.registers.u[decoder::rs()] ^ state.registers.u[decoder::rt()];
        continue;

      case 0x27: // nor rd,rs,rt
        state.registers.u[decoder::rd()] = ~(state.registers.u[decoder::rs()] | state.registers.u[decoder::rt()]);
        continue;

      case 0x2a: // slt rd,rs,rt
        state.registers.i[decoder::rd()] = state.registers.i[decoder::rs()] < state.registers.i[decoder::rt()];
        continue;

      case 0x2b: // sltu rd,rs,rt
        state.registers.i[decoder::rd()] = state.registers.u[decoder::rs()] < state.registers.u[decoder::rt()];
        continue;
      }
      break;

    case 0x01: // reg-imm
      switch (decoder::rt()) {
      case 0x00: // bltz rs,$nnnn
        if (state.registers.i[decoder::rs()] < 0) {
          state.registers.next_pc = state.registers.pc + (decoder::iconst() << 2);
        }
        continue;

      case 0x01: // bgez rs,$nnnn
        if (state.registers.i[decoder::rs()] >= 0) {
          state.registers.next_pc = state.registers.pc + (decoder::iconst() << 2);
        }
        continue;

      case 0x10: { // bltzal rs,$nnnn
        bool condition = state.registers.i[decoder::rs()] < 0;
        state.registers.u[31] = state.registers.next_pc;

        if (condition) {
          state.registers.next_pc = state.registers.pc + (decoder::iconst() << 2);
        }
        continue;
      }

      case 0x11: { // bgezal rs,$nnnn
        bool condition = state.registers.i[decoder::rs()] >= 0;
        state.registers.u[31] = state.registers.next_pc;

        if (condition) {
          state.registers.next_pc = state.registers.pc + (decoder::iconst() << 2);
        }
        continue;
      }
      }
      break;

    case 0x02: // j $3ffffff
      state.registers.next_pc = (state.registers.pc & 0xf0000000) | ((decoder::code << 2) & 0x0ffffffc);
      continue;

    case 0x03: // jal $3ffffff
      state.registers.u[31] = state.registers.next_pc;
      state.registers.next_pc = (state.registers.pc & 0xf0000000) | ((decoder::code << 2) & 0x0ffffffc);
      continue;

    case 0x04: // beq rs,rt,$nnnn
      if (state.registers.u[decoder::rs()] == state.registers.u[decoder::rt()]) {
        state.registers.next_pc = state.registers.pc + (decoder::iconst() << 2);
      }
      continue;

    case 0x05: // bne rs,rt,$nnnn
      if (state.registers.u[decoder::rs()] != state.registers.u[decoder::rt()]) {
        state.registers.next_pc = state.registers.pc + (decoder::iconst() << 2);
      }
      continue;

    case 0x06: // blez rs,$nnnn
      if (state.registers.i[decoder::rs()] <= 0) {
        state.registers.next_pc = state.registers.pc + (decoder::iconst() << 2);
      }
      continue;

    case 0x07: // bgtz rs,$nnnn
      if (state.registers.i[decoder::rs()] > 0) {
        state.registers.next_pc = state.registers.pc + (decoder::iconst() << 2);
      }
      continue;

    case 0x08: // addi rt,rs,$nnnn
      state.registers.u[decoder::rt()] = state.registers.u[decoder::rs()] + decoder::iconst();
      // todo: overflow exception
      continue;

    case 0x09: // addiu rt,rs,$nnnn
      state.registers.u[decoder::rt()] = state.registers.u[decoder::rs()] + decoder::iconst();
      continue;

    case 0x0a: // slti rt,rs,$nnnn
      state.registers.i[decoder::rt()] = state.registers.i[decoder::rs()] < decoder::iconst();
      continue;

    case 0x0b: // sltiu rt,rs,$nnnn
      state.registers.i[decoder::rt()] = state.registers.u[decoder::rs()] < uint32_t(decoder::iconst());
      continue;

    case 0x0c: // andi rt,rs,$nnnn
      state.registers.u[decoder::rt()] = state.registers.u[decoder::rs()] & decoder::uconst();
      continue;

    case 0x0d: // ori rt,rs,$nnnn
      state.registers.u[decoder::rt()] = state.registers.u[decoder::rs()] | decoder::uconst();
      continue;

    case 0x0e: // xori rt,rs,$nnnn
      state.registers.u[decoder::rt()] = state.registers.u[decoder::rs()] ^ decoder::uconst();
      continue;

    case 0x0f: // lui rt,$nnnn
      state.registers.u[decoder::rt()] = decoder::uconst() << 16;
      continue;

    case 0x10: // cop0
      switch (decoder::rs()) {
      case 0x00: // mfc0 rt,rd
        state.registers.u[decoder::rt()] = state.cop0.registers[decoder::rd()];
        continue;

      case 0x04: // mtc0 rt,rd
        state.cop0.registers[decoder::rd()] = state.registers.u[decoder::rt()];
        continue;

      case 0x10:
        switch (decoder::op_lo()) {
        case 0x01: utility::debug("unimplemented tlbr\n"); return;
        case 0x02: utility::debug("unimplemented tlbwi\n"); return;
        case 0x06: utility::debug("unimplemented tblwr\n"); return;
        case 0x08: utility::debug("unimplemented tlbp\n"); return;
        case 0x10: // rfe
          leave_exception();
          continue;
        }
        break;
      }
      break;

    case 0x11: // cop1
      utility::debug("unimplemented cop1\n");
      break;

    case 0x12: // cop2
      utility::debug("unimplemented cop2\n");
      break;

    case 0x13: // cop3
      utility::debug("unimplemented cop3\n");
      break;

    case 0x20: // lb rt,$nnnn(rs)
      state.registers.i[decoder::rt()] = int8_t(read_data(BYTE, state.registers.u[decoder::rs()] + decoder::iconst()));
      continue;

    case 0x21: // lh rt,$nnnn(rs)
      state.registers.i[decoder::rt()] = int16_t(read_data(HALF, state.registers.u[decoder::rs()] + decoder::iconst()));
      continue;

    case 0x22: // lwl rt,$nnnn(rs)
      utility::debug("unimplemented lwl\n");
      continue;

    case 0x23: // lw rt,$nnnn(rs)
      state.registers.u[decoder::rt()] = read_data(WORD, state.registers.u[decoder::rs()] + decoder::iconst());
      continue;

    case 0x24: // lbu rt,$nnnn(rs)
      state.registers.u[decoder::rt()] = read_data(BYTE, state.registers.u[decoder::rs()] + decoder::iconst());
      continue;

    case 0x25: // lhu rt,$nnnn(rs)
      state.registers.u[decoder::rt()] = read_data(HALF, state.registers.u[decoder::rs()] + decoder::iconst());
      continue;

    case 0x26: // lwr rt,$nnnn(rs)
      utility::debug("unimplemented lwr\n");
      break;

    case 0x28: // sb rt,$nnnn(rs)
      write_data(BYTE, state.registers.u[decoder::rs()] + decoder::iconst(), state.registers.u[decoder::rt()]);
      continue;

    case 0x29: // sh rt,$nnnn(rs)
      write_data(HALF, state.registers.u[decoder::rs()] + decoder::iconst(), state.registers.u[decoder::rt()]);
      continue;

    case 0x2a: // swl rt,$nnnn(rs)
      utility::debug("unimplemented swl\n");
      break;

    case 0x2b: // sw rt,$nnnn(rs)
      write_data(WORD, state.registers.u[decoder::rs()] + decoder::iconst(), state.registers.u[decoder::rt()]);
      continue;

    case 0x2e: // swr rt,$nnnn(rs)
      utility::debug("unimplemented swr\n");
      break;

    case 0x30: // lwc0 rt,$nnnn(rs)
    case 0x31: // lwc1 rt,$nnnn(rs)
    case 0x32: // lwc2 rt,$nnnn(rs)
    case 0x33: // lwc3 rt,$nnnn(rs)
      utility::debug("unimplemented lwc\n");
      break;

    case 0x38: // swc0 rt,$nnnn(rs)
    case 0x39: // swc1 rt,$nnnn(rs)
    case 0x3a: // swc2 rt,$nnnn(rs)
    case 0x3b: // swc3 rt,$nnnn(rs)
      utility::debug("unimplemented swc\n");
      break;
    }
  }
}

void cpu::enter_exception(uint32_t code, uint32_t epc) {
  auto &cop0 = state.cop0.registers;

  uint32_t status = cop0[12];
  status = (status & ~0x3f) | ((status << 2) & 0x3f);

  uint32_t cause = cop0[13];
  cause = (cause & ~0x7f) | ((code << 2) & 0x7f);

  if (state.is_branch_delay_slot) {
    epc -= 4;
    cause |= 0x80000000;
  }
  else {
    cause &= 0x7fffffff;
  }

  cop0[12] = status;
  cop0[13] = cause;
  cop0[14] = epc;

  state.registers.pc = (status & (1 << 22))
    ? 0xbfc00180
    : 0x80000080
    ;

  state.registers.next_pc = state.registers.pc + 4;
}

void cpu::leave_exception() {
  auto &cop0 = state.cop0.registers;

  uint32_t sr = cop0[12];
  sr = (sr & ~0xf) | ((sr >> 2) & 0xf);

  cop0[12] = sr;
}
