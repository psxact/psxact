#include "cpu_core.hpp"
#include "../bus.hpp"
#include "../utility.hpp"

static void (*op_table_special[64])() = {
  cpu::op_sll,  cpu::op_und,   cpu::op_srl,  cpu::op_sra,  cpu::op_sllv,    cpu::op_und,   cpu::op_srlv, cpu::op_srav,
  cpu::op_jr,   cpu::op_jalr,  cpu::op_und,  cpu::op_und,  cpu::op_syscall, cpu::op_break, cpu::op_und,  cpu::op_und,
  cpu::op_mfhi, cpu::op_mthi,  cpu::op_mflo, cpu::op_mtlo, cpu::op_und,     cpu::op_und,   cpu::op_und,  cpu::op_und,
  cpu::op_mult, cpu::op_multu, cpu::op_div,  cpu::op_divu, cpu::op_und,     cpu::op_und,   cpu::op_und,  cpu::op_und,
  cpu::op_add,  cpu::op_addu,  cpu::op_sub,  cpu::op_subu, cpu::op_and,     cpu::op_or,    cpu::op_xor,  cpu::op_nor,
  cpu::op_und,  cpu::op_und,   cpu::op_slt,  cpu::op_sltu, cpu::op_und,     cpu::op_und,   cpu::op_und,  cpu::op_und,
  cpu::op_und,  cpu::op_und,   cpu::op_und,  cpu::op_und,  cpu::op_und,     cpu::op_und,   cpu::op_und,  cpu::op_und,
  cpu::op_und,  cpu::op_und,   cpu::op_und,  cpu::op_und,  cpu::op_und,     cpu::op_und,   cpu::op_und,  cpu::op_und
};

// --========--
//   Decoding
// --========--

static inline uint32_t overflow(uint32_t x, uint32_t y, uint32_t z) {
  return (~(x ^ y) & (x ^ z) & 0x80000000);
}

template<bool forwarded = false>
static inline uint32_t get_register(uint32_t index) {
  if (!forwarded && cpu::state.is_load_delay_slot && cpu::state.load_index == index) {
    return cpu::state.load_value;
  } else {
    return cpu::state.regs.gp[index];
  }
}

static inline void set_rd(uint32_t value) {
  cpu::state.regs.gp[cpu::decoder::rd()] = value;
  cpu::state.regs.gp[0] = 0;
}

template<bool load = false>
static inline void set_rt(uint32_t value) {
  auto t = cpu::decoder::rt();

  if (load) {
    if (cpu::state.is_load_delay_slot && cpu::state.load_index == t) {
      cpu::state.regs.gp[t] = cpu::state.load_value;
    }

    cpu::state.is_load = true;
    cpu::state.load_index = t;
    cpu::state.load_value = cpu::state.regs.gp[t];
  }

  cpu::state.regs.gp[t] = value;
  cpu::state.regs.gp[0] = 0;
}

template<bool forwarded = false>
static inline uint32_t rt() {
  return get_register<forwarded>(cpu::decoder::rt());
}

static inline uint32_t rs() {
  return get_register(cpu::decoder::rs());
}

// --============--
//   Instructions
// --============--

void cpu::op_add() {
  auto x = rs();
  auto y = rt();
  auto z = x + y;

  if (overflow(x, y, z)) {
    enter_exception(0xc);
  } else {
    set_rd(z);
  }
}

void cpu::op_addi() {
  auto x = rs();
  auto y = decoder::iconst();
  auto z = x + y;

  if (overflow(x, y, z)) {
    enter_exception(0xc);
  } else {
    set_rt(z);
  }
}

void cpu::op_addiu() {
  set_rt(rs() + decoder::iconst());
}

void cpu::op_addu() {
  set_rd(rs() + rt());
}

void cpu::op_and() {
  set_rd(rs() & rt());
}

void cpu::op_andi() {
  set_rt(rs() & decoder::uconst());
}

void cpu::op_beq() {
  if (rs() == rt()) {
    state.regs.next_pc = state.regs.pc + (decoder::iconst() << 2);
    state.is_branch = true;
  }
}

void cpu::op_reg_imm() {
  // bgez rs,$nnnn
  // bgezal rs,$nnnn
  // bltz rs,$nnnn
  // bltzal rs,$nnnn
  bool condition = (state.code & (1 << 16))
                   ? int32_t(rs()) >= 0
                   : int32_t(rs()) <  0;

  if ((state.code & 0x1e0000) == 0x100000) {
    state.regs.gp[31] = state.regs.next_pc;
  }

  if (condition) {
    state.regs.next_pc = state.regs.pc + (decoder::iconst() << 2);
    state.is_branch = true;
  }
}

void cpu::op_bgtz() {
  if (int32_t(rs()) > 0) {
    state.regs.next_pc = state.regs.pc + (decoder::iconst() << 2);
    state.is_branch = true;
  }
}

void cpu::op_blez() {
  if (int32_t(rs()) <= 0) {
    state.regs.next_pc = state.regs.pc + (decoder::iconst() << 2);
    state.is_branch = true;
  }
}

void cpu::op_bne() {
  if (rs() != rt()) {
    state.regs.next_pc = state.regs.pc + (decoder::iconst() << 2);
    state.is_branch = true;
  }
}

void cpu::op_break() {
  enter_exception(0x09);
}

void cpu::op_cop0() {
  switch ((cpu::state.code >> 21) & 31) {
    default: op_und(); return;

    case 0x00: set_rt(state.cop0.regs[decoder::rd()]); return; // mfc0 rt,rd
    case 0x04: state.cop0.regs[decoder::rd()] = rt(); return; // mtc0 rt,rd

    case 0x10:
      switch (cpu::state.code & 63) {
        default: op_und(); return;

        case 0x10: leave_exception(); return; // rfe
      }
  }
}

void cpu::op_cop1() {
  enter_exception(0xb);
}

void cpu::op_cop2() {
  throw "unimplemented cop2\n";
}

void cpu::op_cop3() {
  enter_exception(0xb);
}

void cpu::op_div() {
  auto dividend = int32_t(rs());
  auto divisor = int32_t(rt());

  if (dividend == 0x80000000 && divisor == 0xffffffff) {
    state.regs.lo = 0x80000000;
    state.regs.hi = 0;
  } else if (dividend >= 0 && divisor == 0) {
    state.regs.lo = uint32_t(0xffffffff);
    state.regs.hi = uint32_t(dividend);
  } else if (dividend <= 0 && divisor == 0) {
    state.regs.lo = uint32_t(0x00000001);
    state.regs.hi = uint32_t(dividend);
  } else {
    state.regs.lo = uint32_t(dividend / divisor);
    state.regs.hi = uint32_t(dividend % divisor);
  }
}

void cpu::op_divu() {
  auto dividend = rs();
  auto divisor = rt();

  if (divisor) {
    state.regs.lo = dividend / divisor;
    state.regs.hi = dividend % divisor;
  } else {
    state.regs.lo = 0xffffffff;
    state.regs.hi = dividend;
  }
}

void cpu::op_j() {
  state.regs.next_pc = (state.regs.pc & 0xf0000000) | ((state.code << 2) & 0x0ffffffc);
  state.is_branch = true;
}

void cpu::op_jal() {
  state.regs.gp[31] = state.regs.next_pc;
  state.regs.next_pc = (state.regs.pc & 0xf0000000) | ((state.code << 2) & 0x0ffffffc);
  state.is_branch = true;
}

void cpu::op_jalr() {
  auto ra = state.regs.next_pc;

  state.regs.next_pc = rs();
  set_rd(ra);

  state.is_branch = true;
}

void cpu::op_jr() {
  state.regs.next_pc = rs();
  state.is_branch = true;
}

void cpu::op_lb() {
  auto address = rs() + decoder::iconst();
  auto data = read_data_byte(address);
  data = utility::sclip<8>(data);

  set_rt<1>(data);
}

void cpu::op_lbu() {
  auto address = rs() + decoder::iconst();
  auto data = read_data_byte(address);

  set_rt<1>(data);
}

void cpu::op_lh() {
  auto address = rs() + decoder::iconst();
  if (address & 1) {
    enter_exception(0x4);
  } else {
    auto data = read_data_half(address);
    data = utility::sclip<16>(data);

    set_rt<1>(data);
  }
}

void cpu::op_lhu() {
  auto address = rs() + decoder::iconst();
  if (address & 1) {
    enter_exception(0x4);
  } else {
    auto data = read_data_half(address);

    set_rt<1>(data);
  }
}

void cpu::op_lui() {
  set_rt(decoder::uconst() << 16);
}

void cpu::op_lw() {
  auto address = rs() + decoder::iconst();
  if (address & 3) {
    enter_exception(0x4);
  } else {
    auto data = read_data_word(address);

    set_rt<1>(data);
  }
}

void cpu::op_lwc0() {
  throw "unimplemented lwc0\n";
}

void cpu::op_lwc1() {
  throw "unimplemented lwc1\n";
}

void cpu::op_lwc2() {
  throw "unimplemented lwc2\n";
}

void cpu::op_lwc3() {
  throw "unimplemented lwc3\n";
}

void cpu::op_lwl() {
  auto address = rs() + decoder::iconst();
  auto data = read_data_word(address & ~3);

  switch (address & 3) {
    default: data = (data << 24) | (rt<1>() & 0x00ffffff); break;
    case  1: data = (data << 16) | (rt<1>() & 0x0000ffff); break;
    case  2: data = (data <<  8) | (rt<1>() & 0x000000ff); break;
    case  3: data = (data <<  0) | (rt<1>() & 0x00000000); break;
  }

  set_rt<1>(data);
}

void cpu::op_lwr() {
  auto address = rs() + decoder::iconst();
  auto data = read_data_word(address & ~3);

  switch (address & 3) {
    default: data = (data >>  0) | (rt<1>() & 0x00000000); break;
    case  1: data = (data >>  8) | (rt<1>() & 0xff000000); break;
    case  2: data = (data >> 16) | (rt<1>() & 0xffff0000); break;
    case  3: data = (data >> 24) | (rt<1>() & 0xffffff00); break;
  }

  set_rt<1>(data);
}

void cpu::op_mfhi() {
  set_rd(state.regs.hi);
}

void cpu::op_mflo() {
  set_rd(state.regs.lo);
}

void cpu::op_mthi() {
  state.regs.hi = rs();
}

void cpu::op_mtlo() {
  state.regs.lo = rs();
}

void cpu::op_mult() {
  auto s = int32_t(rs());
  auto t = int32_t(rt());

  int64_t result = int64_t(s) * int64_t(t);
  state.regs.lo = uint32_t(result >> 0);
  state.regs.hi = uint32_t(result >> 32);
}

void cpu::op_multu() {
  auto s = rs();
  auto t = rt();

  uint64_t result = uint64_t(s) * uint64_t(t);
  state.regs.lo = uint32_t(result >> 0);
  state.regs.hi = uint32_t(result >> 32);
}

void cpu::op_nor() {
  set_rd(~(rs() | rt()));
}

void cpu::op_or() {
  set_rd(rs() | rt());
}

void cpu::op_ori() {
  set_rt(rs() | decoder::uconst());
}

void cpu::op_sb() {
  auto address = rs() + decoder::iconst();
  auto data = rt();

  write_data_byte(address, data);
}

void cpu::op_sh() {
  auto address = rs() + decoder::iconst();
  if (address & 1) {
    enter_exception(0x5);
  } else {
    auto data = rt();

    write_data_half(address, data);
  }
}

void cpu::op_sll() {
  set_rd(rt() << decoder::sa());
}

void cpu::op_sllv() {
  set_rd(rt() << rs());
}

void cpu::op_slt() {
  set_rd(int32_t(rs()) < int32_t(rt()) ? 1 : 0);
}

void cpu::op_slti() {
  set_rt(int32_t(rs()) < int32_t(decoder::iconst()) ? 1 : 0);
}

void cpu::op_sltiu() {
  set_rt(rs() < decoder::iconst() ? 1 : 0);
}

void cpu::op_sltu() {
  set_rd(rs() < rt() ? 1 : 0);
}

void cpu::op_special() {
  op_table_special[cpu::state.code & 63]();
}

void cpu::op_sra() {
  set_rd(int32_t(rt()) >> decoder::sa());
}

void cpu::op_srav() {
  set_rd(int32_t(rt()) >> rs());
}

void cpu::op_srl() {
  set_rd(rt() >> decoder::sa());
}

void cpu::op_srlv() {
  set_rd(rt() >> rs());
}

void cpu::op_sub() {
  auto x = rs();
  auto y = rt();
  auto z = x - y;

  if (overflow(x, ~y, z)) {
    enter_exception(0xc);
  } else {
    set_rd(z);
  }
}

void cpu::op_subu() {
  set_rd(rs() - rt());
}

void cpu::op_sw() {
  auto address = rs() + decoder::iconst();
  if (address & 3) {
    enter_exception(0x5);
  } else {
    auto data = rt();

    write_data_word(address, data);
  }
}

void cpu::op_swc0() {
  throw "unimplemented swc0\n";
}

void cpu::op_swc1() {
  throw "unimplemented swc1\n";
}

void cpu::op_swc2() {
  throw "unimplemented swc2\n";
}

void cpu::op_swc3() {
  throw "unimplemented swc3\n";
}

void cpu::op_swl() {
  auto address = rs() + decoder::iconst();
  auto data = read_data_word(address & ~3);

  switch (address & 3) {
    default: data = (data & 0xffffff00) | (rt() >> 24); break;
    case  1: data = (data & 0xffff0000) | (rt() >> 16); break;
    case  2: data = (data & 0xff000000) | (rt() >>  8); break;
    case  3: data = (data & 0x00000000) | (rt() >>  0); break;
  }

  write_data_word(address & ~3, data);
}

void cpu::op_swr() {
  auto address = rs() + decoder::iconst();
  auto data = read_data_word(address & ~3);

  switch (address & 3) {
    default: data = (data & 0x00000000) | (rt() <<  0); break;
    case  1: data = (data & 0x000000ff) | (rt() <<  8); break;
    case  2: data = (data & 0x0000ffff) | (rt() << 16); break;
    case  3: data = (data & 0x00ffffff) | (rt() << 24); break;
  }

  write_data_word(address & ~3, data);
}

void cpu::op_syscall() {
  enter_exception(0x08);
}

void cpu::op_xor() {
  set_rd(rs() ^ rt());
}

void cpu::op_xori() {
  set_rt(rs() ^ decoder::uconst());
}

void cpu::op_und() {
  cpu::enter_exception(0xa);
}
