#include "cpu_core.hpp"
#include "../bus.hpp"

static cpu::state_t state;

static inline uint32_t uconst() {
  return ((state.code & 0xffff) ^ 0x0000) - 0x0000;
}

static inline uint32_t iconst() {
  return ((state.code & 0xffff) ^ 0x8000) - 0x8000;
}

static inline uint32_t op_lo() {
  return (state.code >>  0) & 63;
}

static inline uint32_t sa() {
  return (state.code >>  6) & 31;
}

static inline uint32_t rd() {
  return (state.code >> 11) & 31;
}

static inline uint32_t rt() {
  return (state.code >> 16) & 31;
}

static inline uint32_t rs() {
  return (state.code >> 21) & 31;
}

static inline uint32_t op_hi() {
  return (state.code >> 26) & 63;
}

void cpu::initialize() {
  state.registers.u[0] = 0;
  state.registers.pc = 0xbfc00000;
  state.registers.next_pc = state.registers.pc + 4;
}

void invalid_instruction() {
  printf("unhandled code: $%08x\n", state.code);
}

void cpu::main() {
  while (1) {
    state.code = cpu::read_code();

    state.is_branch_delay_slot = state.is_branch;
    state.is_branch = false;

    switch (op_hi()) {
      default:
        invalid_instruction();
        return;

      case 0x00: // special
        switch (op_lo()) {
          default:
            invalid_instruction();
            return;

          case 0x00: // sll rd,rt,sa
            state.registers.u[rd()] = state.registers.u[rt()] << sa();
            continue;

          case 0x02: // srl rd,rt,sa
            state.registers.u[rd()] = state.registers.u[rt()] >> sa();
            continue;

          case 0x03: // sra rd,rt,sa
            state.registers.i[rd()] = state.registers.i[rt()] >> sa();
            continue;

          case 0x04: // sllv rd,rt,rs
            state.registers.u[rd()] = state.registers.u[rt()] << state.registers.u[rs()];
            continue;

          case 0x06: // srlv rd,rt,rs
            state.registers.u[rd()] = state.registers.u[rt()] >> state.registers.u[rs()];
            continue;

          case 0x07: // srav rd,rt,rs
            state.registers.i[rd()] = state.registers.i[rt()] >> state.registers.u[rs()];
            continue;

          case 0x08: // jr rs
            state.registers.next_pc = state.registers.u[rs()];
            state.is_branch = true;
            continue;

          case 0x09: // jalr rd,rs
            state.registers.u[rd()] = state.registers.next_pc;
            state.registers.next_pc = state.registers.u[rs()];
            state.is_branch = true;
            continue;

          case 0x0c: // syscall
            enter_exception(0x08, state.registers.pc - 4);
            continue;

          case 0x0d: // break
            enter_exception(0x09, state.registers.pc - 4);
            continue;

          case 0x10: // mfhi rd
            state.registers.u[rd()] = state.registers.hi;
            continue;

          case 0x11: // mthi rs
            state.registers.hi = state.registers.u[rs()];
            continue;

          case 0x12: // mflo rd
            state.registers.u[rd()] = state.registers.lo;
            continue;

          case 0x13: // mtlo rs
            state.registers.lo = state.registers.u[rs()];
            continue;

          case 0x18: // mult rs,rt
          {
            int64_t result = int64_t(state.registers.i[rs()]) * int64_t(state.registers.i[rt()]);
            state.registers.lo = uint32_t(result >> 0);
            state.registers.hi = uint32_t(result >> 32);
            continue;
          }

          case 0x19: // multu rs,rt
          {
            uint64_t result = uint64_t(state.registers.u[rs()]) * uint64_t(state.registers.u[rt()]);
            state.registers.lo = uint32_t(result >> 0);
            state.registers.hi = uint32_t(result >> 32);
            continue;
          }

          case 0x1a: // div rs,rt
            if (state.registers.u[rt()]) {
              state.registers.lo = uint32_t(state.registers.i[rs()] / state.registers.i[rt()]);
              state.registers.hi = uint32_t(state.registers.i[rs()] % state.registers.i[rt()]);
            } else {
              state.registers.lo = ((state.registers.u[rs()] >> 30) & 2) - 1;
              state.registers.hi = state.registers.u[rs()];
            }
            continue;

          case 0x1b: // divu rs,rt
            if (state.registers.u[rt()]) {
              state.registers.lo = state.registers.u[rs()] / state.registers.u[rt()];
              state.registers.hi = state.registers.u[rs()] % state.registers.u[rt()];
            } else {
              state.registers.lo = 0xffffffff;
              state.registers.hi = state.registers.u[rs()];
            }
            continue;

          case 0x20: // add rd,rs,rt
            state.registers.u[rd()] = state.registers.u[rs()] + state.registers.u[rt()];
            // todo: overflow exception
            continue;

          case 0x21: // addu rd,rs,rt
            state.registers.u[rd()] = state.registers.u[rs()] + state.registers.u[rt()];
            continue;

          case 0x22: // sub rd,rs,rt
            state.registers.u[rd()] = state.registers.u[rs()] - state.registers.u[rt()];
            // todo: overflow exception
            continue;

          case 0x23: // subu rd,rs,rt
            state.registers.u[rd()] = state.registers.u[rs()] - state.registers.u[rt()];
            continue;

          case 0x24: // and rd,rs,rt
            state.registers.u[rd()] = state.registers.u[rs()] & state.registers.u[rt()];
            continue;

          case 0x25: // or rd,rs,rt
            state.registers.u[rd()] = state.registers.u[rs()] | state.registers.u[rt()];
            continue;

          case 0x26: // xor rd,rs,rt
            state.registers.u[rd()] = state.registers.u[rs()] ^ state.registers.u[rt()];
            continue;

          case 0x27: // nor rd,rs,rt
            state.registers.u[rd()] = ~(state.registers.u[rs()] | state.registers.u[rt()]);
            continue;

          case 0x2a: // slt rd,rs,rt
            state.registers.i[rd()] = state.registers.i[rs()] < state.registers.i[rt()];
            continue;

          case 0x2b: // sltu rd,rs,rt
            state.registers.i[rd()] = state.registers.u[rs()] < state.registers.u[rt()];
            continue;
        }

      case 0x01: // reg-imm
        switch (rt()) {
          default:
            invalid_instruction();
            return;

          case 0x00: // bltz rs,$nnnn
            if (state.registers.i[rs()] < 0) {
              state.registers.next_pc = state.registers.pc + (iconst() << 2);
              state.is_branch = true;
            }
            continue;

          case 0x01: // bgez rs,$nnnn
            if (state.registers.i[rs()] >= 0) {
              state.registers.next_pc = state.registers.pc + (iconst() << 2);
              state.is_branch = true;
            }
            continue;

          case 0x10: { // bltzal rs,$nnnn
            bool condition = state.registers.i[rs()] < 0;
            state.registers.u[31] = state.registers.next_pc;

            if (condition) {
              state.registers.next_pc = state.registers.pc + (iconst() << 2);
              state.is_branch = true;
            }
            continue;
          }

          case 0x11: { // bgezal rs,$nnnn
            bool condition = state.registers.i[rs()] >= 0;
            state.registers.u[31] = state.registers.next_pc;

            if (condition) {
              state.registers.next_pc = state.registers.pc + (iconst() << 2);
              state.is_branch = true;
            }
            continue;
          }
        }

      case 0x02: // j $3ffffff
        state.registers.next_pc = (state.registers.pc & 0xf0000000) | ((state.code << 2) & 0x0ffffffc);
        state.is_branch = true;
        continue;

      case 0x03: // jal $3ffffff
        state.registers.u[31] = state.registers.next_pc;
        state.registers.next_pc = (state.registers.pc & 0xf0000000) | ((state.code << 2) & 0x0ffffffc);
        state.is_branch = true;
        continue;

      case 0x04: // beq rs,rt,$nnnn
        if (state.registers.u[rs()] == state.registers.u[rt()]) {
          state.registers.next_pc = state.registers.pc + (iconst() << 2);
          state.is_branch = true;
        }
        continue;

      case 0x05: // bne rs,rt,$nnnn
        if (state.registers.u[rs()] != state.registers.u[rt()]) {
          state.registers.next_pc = state.registers.pc + (iconst() << 2);
          state.is_branch = true;
        }
        continue;

      case 0x06: // blez rs,$nnnn
        if (state.registers.i[rs()] <= 0) {
          state.registers.next_pc = state.registers.pc + (iconst() << 2);
          state.is_branch = true;
        }
        continue;

      case 0x07: // bgtz rs,$nnnn
        if (state.registers.i[rs()] > 0) {
          state.registers.next_pc = state.registers.pc + (iconst() << 2);
          state.is_branch = true;
        }
        continue;

      case 0x08: // addi rt,rs,$nnnn
        state.registers.u[rt()] = state.registers.u[rs()] + iconst();
        // todo: overflow exception
        continue;

      case 0x09: // addiu rt,rs,$nnnn
        state.registers.u[rt()] = state.registers.u[rs()] + iconst();
        continue;

      case 0x0a: // slti rt,rs,$nnnn
        state.registers.i[rt()] = state.registers.i[rs()] < int32_t(iconst());
        continue;

      case 0x0b: // sltiu rt,rs,$nnnn
        state.registers.i[rt()] = state.registers.u[rs()] < iconst();
        continue;

      case 0x0c: // andi rt,rs,$nnnn
        state.registers.u[rt()] = state.registers.u[rs()] & uconst();
        continue;

      case 0x0d: // ori rt,rs,$nnnn
        state.registers.u[rt()] = state.registers.u[rs()] | uconst();
        continue;

      case 0x0e: // xori rt,rs,$nnnn
        state.registers.u[rt()] = state.registers.u[rs()] ^ uconst();
        continue;

      case 0x0f: // lui rt,$nnnn
        state.registers.u[rt()] = uconst() << 16;
        continue;

      case 0x10: // cop0
        switch (rs()) {
          default:
            invalid_instruction();
            return;

          case 0x00: // mfc0 rt,rd
            state.registers.u[rt()] = state.cop0.registers[rd()];
            continue;

          case 0x04: // mtc0 rt,rd
            state.cop0.registers[rd()] = state.registers.u[rt()];
            continue;

          case 0x10:
            switch (op_lo()) {
              default:
                invalid_instruction();
                return;

              case 0x10: // rfe
                leave_exception();
                continue;
            }
        }

      case 0x11: // cop1
        printf("unimplemented cop1\n");
        return;

      case 0x12: // cop2
        printf("unimplemented cop2\n");
        return;

      case 0x13: // cop3
        printf("unimplemented cop3\n");
        return;

      case 0x20: // lb rt,$nnnn(rs)
        state.registers.i[rt()] = int8_t(read_data(BYTE, state.registers.u[rs()] + iconst()));
        continue;

      case 0x21: // lh rt,$nnnn(rs)
        state.registers.i[rt()] = int16_t(read_data(HALF, state.registers.u[rs()] + iconst()));
        continue;

      case 0x22: // lwl rt,$nnnn(rs)
        printf("unimplemented lwl\n");
        return;

      case 0x23: // lw rt,$nnnn(rs)
        state.registers.u[rt()] = read_data(WORD, state.registers.u[rs()] + iconst());
        continue;

      case 0x24: // lbu rt,$nnnn(rs)
        state.registers.u[rt()] = read_data(BYTE, state.registers.u[rs()] + iconst());
        continue;

      case 0x25: // lhu rt,$nnnn(rs)
        state.registers.u[rt()] = read_data(HALF, state.registers.u[rs()] + iconst());
        continue;

      case 0x26: // lwr rt,$nnnn(rs)
        printf("unimplemented lwr\n");
        return;

      case 0x28: // sb rt,$nnnn(rs)
        write_data(BYTE, state.registers.u[rs()] + iconst(), state.registers.u[rt()]);
        continue;

      case 0x29: // sh rt,$nnnn(rs)
        write_data(HALF, state.registers.u[rs()] + iconst(), state.registers.u[rt()]);
        continue;

      case 0x2a: // swl rt,$nnnn(rs)
        printf("unimplemented swl\n");
        return;

      case 0x2b: // sw rt,$nnnn(rs)
        write_data(WORD, state.registers.u[rs()] + iconst(), state.registers.u[rt()]);
        continue;

      case 0x2e: // swr rt,$nnnn(rs)
        printf("unimplemented swr\n");
        return;

      case 0x30: // lwc0 rt,$nnnn(rs)
      case 0x31: // lwc1 rt,$nnnn(rs)
      case 0x32: // lwc2 rt,$nnnn(rs)
      case 0x33: // lwc3 rt,$nnnn(rs)
        printf("unimplemented lwc\n");
        return;

      case 0x38: // swc0 rt,$nnnn(rs)
      case 0x39: // swc1 rt,$nnnn(rs)
      case 0x3a: // swc2 rt,$nnnn(rs)
      case 0x3b: // swc3 rt,$nnnn(rs)
        printf("unimplemented swc\n");
        return;
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
  } else {
    cause &= 0x7fffffff;
  }

  cop0[12] = status;
  cop0[13] = cause;
  cop0[14] = epc;

  state.registers.pc = (status & (1 << 22))
                       ? 0xbfc00180
                       : 0x80000080;

  state.registers.next_pc = state.registers.pc + 4;
}

void cpu::leave_exception() {
  auto &cop0 = state.cop0.registers;

  uint32_t sr = cop0[12];
  sr = (sr & ~0xf) | ((sr >> 2) & 0xf);

  cop0[12] = sr;
}

static uint32_t segments[8] = {
    0x7fffffff, // kuseg ($0000_0000 - $7fff_ffff)
    0x7fffffff, //
    0x7fffffff, //
    0x7fffffff, //
    0x1fffffff, // kseg0 ($8000_0000 - $9fff_ffff)
    0x1fffffff, // kseg1 ($a000_0000 - $bfff_ffff)
    0x3fffffff, // kseg2 ($c000_0000 - $ffff_ffff)
    0xffffffff   //
};

static inline uint32_t map_address(uint32_t address) {
  return address & segments[address >> 29];
}

uint32_t cpu::read_code() {
  auto address = state.registers.pc;

  state.registers.pc = state.registers.next_pc;
  state.registers.next_pc += 4;

  // todo: mmio_read i-cache

  return bus::read(WORD, map_address(address));
}

uint32_t cpu::read_data(int size, uint32_t address) {
  if (state.cop0.registers[12] & (1 << 16)) {
    return 0; // isc=1
  }

  // todo: mmio_read d-cache

  return bus::read(size, map_address(address));
}

void cpu::write_data(int size, uint32_t address, uint32_t data) {
  if (state.cop0.registers[12] & (1 << 16)) {
    return; // isc=1
  }

  // todo: mmio_write d-cache

  return bus::write(size, map_address(address), data);
}
