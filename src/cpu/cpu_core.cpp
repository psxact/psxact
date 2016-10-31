#include "cpu_core.hpp"
#include "../bus.hpp"

static cpu::state_t state;

static inline uint32_t uconst() {
  return ((state.code & 0xffff) ^ 0x0000) - 0x0000;
}

static inline uint32_t iconst() {
  return ((state.code & 0xffff) ^ 0x8000) - 0x8000;
}

static inline uint32_t sa() {
  return (state.code >>  6) & 31;
}

static inline void set_rd(uint32_t value) {
  state.regs.gp[(state.code >> 11) & 31] = value;
  state.regs.gp[0] = 0;
}

static inline uint32_t rd() {
  return (state.code >> 11) & 31;
}

static inline void set_rt(uint32_t value) {
  state.regs.gp[(state.code >> 16) & 31] = value;
  state.regs.gp[0] = 0;
}

static inline uint32_t rt() {
  return state.regs.gp[(state.code >> 16) & 31];
}

static inline uint32_t rs() {
  return state.regs.gp[(state.code >> 21) & 31];
}

static inline uint32_t op_lo() {
  return (state.code >>  0) & 63;
}

static inline uint32_t op_ri() {
  return (state.code >> 16) & 31;
}

static inline uint32_t op_cp() {
  return (state.code >> 21) & 31;
}

static inline uint32_t op_hi() {
  return (state.code >> 26) & 63;
}

void cpu::initialize() {
  state.regs.gp[0] = 0;
  state.regs.pc = 0xbfc00000;
  state.regs.next_pc = state.regs.pc + 4;
}

static inline void invalid_instruction() {
  printf("unhandled code: $%08x\n", state.code);
}

static inline uint32_t overflow(uint32_t x, uint32_t y, uint32_t z) {
  return (~(x ^ y) & (x ^ z) & 0x80000000);
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
            set_rd(rt() << sa());
            continue;

          case 0x02: // srl rd,rt,sa
            set_rd(rt() >> sa());
            continue;

          case 0x03: // sra rd,rt,sa
            set_rd(int32_t(rt()) >> sa());
            continue;

          case 0x04: // sllv rd,rt,rs
            set_rd(rt() << rs());
            continue;

          case 0x06: // srlv rd,rt,rs
            set_rd(rt() >> rs());
            continue;

          case 0x07: // srav rd,rt,rs
            set_rd(int32_t(rt()) >> rs());
            continue;

          case 0x08: // jr rs
            state.regs.next_pc = rs();
            state.is_branch = true;
            continue;

          case 0x09: { // jalr rd,rs
            auto ra = state.regs.next_pc;

            state.regs.next_pc = rs();
            set_rd(ra);

            state.is_branch = true;
            continue;
          }

          case 0x0c: // syscall
            enter_exception(0x08, state.regs.pc - 4);
            continue;

          case 0x0d: // break
            enter_exception(0x09, state.regs.pc - 4);
            continue;

          case 0x10: // mfhi rd
            set_rd(state.regs.hi);
            continue;

          case 0x11: // mthi rs
            state.regs.hi = rs();
            continue;

          case 0x12: // mflo rd
            set_rd(state.regs.lo);
            continue;

          case 0x13: // mtlo rs
            state.regs.lo = rs();
            continue;

          case 0x18: { // mult rs,rt
            auto s = int32_t(rs());
            auto t = int32_t(rt());

            int64_t result = int64_t(s) * int64_t(t);
            state.regs.lo = uint32_t(result >> 0);
            state.regs.hi = uint32_t(result >> 32);
            continue;
          }

          case 0x19: { // multu rs,rt
            auto s = rs();
            auto t = rt();

            uint64_t result = uint64_t(s) * uint64_t(t);
            state.regs.lo = uint32_t(result >> 0);
            state.regs.hi = uint32_t(result >> 32);
            continue;
          }

          case 0x1a: { // div rs,rt
            auto dividend = int32_t(rs());
            auto divisor = int32_t(rt());
            if (divisor) {
              state.regs.lo = uint32_t(dividend / divisor);
              state.regs.hi = uint32_t(dividend % divisor);
            } else {
              state.regs.lo = uint32_t((dividend >> 30) & 2) - 1;
              state.regs.hi = uint32_t(dividend);
            }
            continue;
          }

          case 0x1b: { // divu rs,rt
            auto dividend = rs();
            auto divisor = rt();
            if (divisor) {
              state.regs.lo = dividend / divisor;
              state.regs.hi = dividend % divisor;
            } else {
              state.regs.lo = 0xffffffff;
              state.regs.hi = dividend;
            }
            continue;
          }

          case 0x20: { // add rd,rs,rt
            auto x = rs();
            auto y = rt();
            auto z = x + y;

            if (overflow(x, y, z)) {
              enter_exception(0xc, state.regs.pc);
            } else {
              set_rd(z);
            }
            continue;
          }

          case 0x21: // addu rd,rs,rt
            set_rd(rs() + rt());
            continue;

          case 0x22: { // sub rd,rs,rt
            auto x = rs();
            auto y = rt();
            auto z = x - y;

            if (overflow(x, y, z)) {
              enter_exception(0xc, state.regs.pc);
            } else {
              set_rd(z);
            }
            continue;
          }

          case 0x23: // subu rd,rs,rt
            set_rd(rs() - rt());
            continue;

          case 0x24: // and rd,rs,rt
            set_rd(rs() & rt());
            continue;

          case 0x25: // or rd,rs,rt
            set_rd(rs() | rt());
            continue;

          case 0x26: // xor rd,rs,rt
            set_rd(rs() ^ rt());
            continue;

          case 0x27: // nor rd,rs,rt
            set_rd(~(rs() | rt()));
            continue;

          case 0x2a: // slt rd,rs,rt
            set_rd(int32_t(rs()) < int32_t(rt()) ? 1 : 0);
            continue;

          case 0x2b: // sltu rd,rs,rt
            set_rd(rs() < rt() ? 1 : 0);
            continue;
        }

      case 0x01: // reg-imm
        switch (op_ri()) {
          default:
            invalid_instruction();
            return;

          case 0x00: // bltz rs,$nnnn
            if (int32_t(rs()) < 0) {
              state.regs.next_pc = state.regs.pc + (iconst() << 2);
              state.is_branch = true;
            }
            continue;

          case 0x01: // bgez rs,$nnnn
            if (int32_t(rs()) >= 0) {
              state.regs.next_pc = state.regs.pc + (iconst() << 2);
              state.is_branch = true;
            }
            continue;

          case 0x10: { // bltzal rs,$nnnn
            bool condition = int32_t(rs()) < 0;
            state.regs.gp[31] = state.regs.next_pc;

            if (condition) {
              state.regs.next_pc = state.regs.pc + (iconst() << 2);
              state.is_branch = true;
            }
            continue;
          }

          case 0x11: { // bgezal rs,$nnnn
            bool condition = int32_t(rs()) >= 0;
            state.regs.gp[31] = state.regs.next_pc;

            if (condition) {
              state.regs.next_pc = state.regs.pc + (iconst() << 2);
              state.is_branch = true;
            }
            continue;
          }
        }

      case 0x02: // j $3ffffff
        state.regs.next_pc = (state.regs.pc & 0xf0000000) | ((state.code << 2) & 0x0ffffffc);
        state.is_branch = true;
        continue;

      case 0x03: // jal $3ffffff
        state.regs.gp[31] = state.regs.next_pc;
        state.regs.next_pc = (state.regs.pc & 0xf0000000) | ((state.code << 2) & 0x0ffffffc);
        state.is_branch = true;
        continue;

      case 0x04: // beq rs,rt,$nnnn
        if (rs() == rt()) {
          state.regs.next_pc = state.regs.pc + (iconst() << 2);
          state.is_branch = true;
        }
        continue;

      case 0x05: // bne rs,rt,$nnnn
        if (rs() != rt()) {
          state.regs.next_pc = state.regs.pc + (iconst() << 2);
          state.is_branch = true;
        }
        continue;

      case 0x06: // blez rs,$nnnn
        if (int32_t(rs()) <= 0) {
          state.regs.next_pc = state.regs.pc + (iconst() << 2);
          state.is_branch = true;
        }
        continue;

      case 0x07: // bgtz rs,$nnnn
        if (int32_t(rs()) > 0) {
          state.regs.next_pc = state.regs.pc + (iconst() << 2);
          state.is_branch = true;
        }
        continue;

      case 0x08: { // addi rt,rs,$nnnn
        auto x = rs();
        auto y = iconst();
        auto z = x + y;

        if (overflow(x, y, z)) {
          enter_exception(0xc, state.regs.pc);
        } else {
          set_rt(z);
        }
        continue;
      }

      case 0x09: // addiu rt,rs,$nnnn
        set_rt(rs() + iconst());
        continue;

      case 0x0a: // slti rt,rs,$nnnn
        set_rt(int32_t(rs()) < int32_t(iconst()) ? 1 : 0);
        continue;

      case 0x0b: // sltiu rt,rs,$nnnn
        set_rt(rs() < iconst() ? 1 : 0);
        continue;

      case 0x0c: // andi rt,rs,$nnnn
        set_rt(rs() & uconst());
        continue;

      case 0x0d: // ori rt,rs,$nnnn
        set_rt(rs() | uconst());
        continue;

      case 0x0e: // xori rt,rs,$nnnn
        set_rt(rs() ^ uconst());
        continue;

      case 0x0f: // lui rt,$nnnn
        set_rt(uconst() << 16);
        continue;

      case 0x10: // cop0
        switch (op_cp()) {
          default:
            invalid_instruction();
            return;

          case 0x00: // mfc0 rt,rd
            set_rt(state.cop0.regs[rd()]);
            continue;

          case 0x04: // mtc0 rt,rd
            state.cop0.regs[rd()] = rt();
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

      case 0x20: { // lb rt,$nnnn(rs)
        auto data = read_data(BYTE, rs() + iconst());
        data = ((data & 0xff) ^ 0x80) - 0x80;

        set_rt(data);
        continue;
      }

      case 0x21: { // lh rt,$nnnn(rs)
        auto data = read_data(HALF, rs() + iconst());
        data = ((data & 0xffff) ^ 0x8000) - 0x8000;

        set_rt(data);
        continue;
      }

      case 0x22: { // lwl rt,$nnnn(rs)
        auto address = rs() + iconst();
        auto data = read_data(WORD, address & ~3);

        switch (address & 3) {
          case 0: set_rt((data << 24) | (rt() & 0x00ffffff)); break;
          case 1: set_rt((data << 16) | (rt() & 0x0000ffff)); break;
          case 2: set_rt((data <<  8) | (rt() & 0x000000ff)); break;
          case 3: set_rt((data <<  0) | (rt() & 0x00000000)); break;
        }

        continue;
      }

      case 0x23: // lw rt,$nnnn(rs)
        set_rt(read_data(WORD, rs() + iconst()));
        continue;

      case 0x24: // lbu rt,$nnnn(rs)
        set_rt(read_data(BYTE, rs() + iconst()));
        continue;

      case 0x25: // lhu rt,$nnnn(rs)
        set_rt(read_data(HALF, rs() + iconst()));
        continue;

      case 0x26: { // lwr rt,$nnnn(rs)
        auto address = rs() + iconst();
        auto data = read_data(WORD, address & ~3);

        switch (address & 3) {
          case 0: set_rt((data >>  0) | (rt() & 0x00000000)); break;
          case 1: set_rt((data >>  8) | (rt() & 0xff000000)); break;
          case 2: set_rt((data >> 16) | (rt() & 0xffff0000)); break;
          case 3: set_rt((data >> 24) | (rt() & 0xffffff00)); break;
        }

        continue;
      }

      case 0x28: // sb rt,$nnnn(rs)
        write_data(BYTE, rs() + iconst(), rt());
        continue;

      case 0x29: // sh rt,$nnnn(rs)
        write_data(HALF, rs() + iconst(), rt());
        continue;

      case 0x2a: { // swl rt,$nnnn(rs)
        auto address = rs() + iconst();
        auto data = read_data(WORD, address & ~3);

        switch (address & 3) {
          case 0: data = (data & 0xffffff00) | (rt() >> 24); break;
          case 1: data = (data & 0xffff0000) | (rt() >> 16); break;
          case 2: data = (data & 0xff000000) | (rt() >>  8); break;
          case 3: data = (data & 0x00000000) | (rt() >>  0); break;
        }

        write_data(WORD, address & ~3, data);
        continue;
      }

      case 0x2b: // sw rt,$nnnn(rs)
        write_data(WORD, rs() + iconst(), rt());
        continue;

      case 0x2e: { // swr rt,$nnnn(rs)
        auto address = rs() + iconst();
        auto data = read_data(WORD, address & ~3);

        switch (address & 3) {
          case 0: data = (data & 0x00000000) | (rt() <<  0); break;
          case 1: data = (data & 0x000000ff) | (rt() <<  8); break;
          case 2: data = (data & 0x0000ffff) | (rt() << 16); break;
          case 3: data = (data & 0x00ffffff) | (rt() << 24); break;
        }

        write_data(WORD, address & ~3, data);
        continue;
      }

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
  uint32_t status = state.cop0.regs[12];
  status = (status & ~0x3f) | ((status << 2) & 0x3f);

  uint32_t cause = state.cop0.regs[13];
  cause = (cause & ~0x7f) | ((code << 2) & 0x7f);

  if (state.is_branch_delay_slot) {
    epc -= 4;
    cause |= (1 << 31);
  } else {
    cause &= ~(1 << 31);
  }

  state.cop0.regs[12] = status;
  state.cop0.regs[13] = cause;
  state.cop0.regs[14] = epc;

  state.regs.pc = (status & (1 << 22))
                       ? 0xbfc00180
                       : 0x80000080;

  state.regs.next_pc = state.regs.pc + 4;
}

void cpu::leave_exception() {
  uint32_t sr = state.cop0.regs[12];
  sr = (sr & ~0xf) | ((sr >> 2) & 0xf);

  state.cop0.regs[12] = sr;
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
  auto address = state.regs.pc;

  state.regs.pc = state.regs.next_pc;
  state.regs.next_pc += 4;

  // todo: read i-cache

  return bus::read(WORD, map_address(address));
}

uint32_t cpu::read_data(int size, uint32_t address) {
  if (state.cop0.regs[12] & (1 << 16)) {
    return 0; // isc=1
  }

  // todo: read d-cache

  return bus::read(size, map_address(address));
}

void cpu::write_data(int size, uint32_t address, uint32_t data) {
  if (state.cop0.regs[12] & (1 << 16)) {
    return; // isc=1
  }

  // todo: write d-cache

  return bus::write(size, map_address(address), data);
}

uint32_t cpu::mmio_read(int size, uint32_t address) {
  switch (address) {
    case 0x1f801070: return state.i_stat;
    case 0x1f801074: return state.i_mask;
  }
}

void cpu::mmio_write(int size, uint32_t address, uint32_t data) {
  switch (address) {
    case 0x1f801070: state.i_stat = state.i_stat & data; break;
    case 0x1f801074: state.i_mask = data; break;
  }
}
