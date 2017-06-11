#include "cpu_core.hpp"
#include "../bus.hpp"
#include "../utility.hpp"
#include "cpu_cop0.hpp"
#include "cpu_cop2.hpp"

// --========--
//   Decoding
// --========--

static inline uint32_t overflow(uint32_t x, uint32_t y, uint32_t z) {
  return (~(x ^ y) & (x ^ z) & 0x80000000);
}

template<bool forwarded = false>
static inline uint32_t get_register(cpu_state_t *state, uint32_t index) {
  if (!forwarded && state->is_load_delay_slot && state->load_index == index) {
    return state->load_value;
  }
  else {
    return state->regs.gp[index];
  }
}

static inline void set_rd(cpu_state_t *state, uint32_t value) {
  state->regs.gp[cpu::decoder::rd(state)] = value;
  state->regs.gp[0] = 0;
}

template<bool load = false>
static inline void set_rt(cpu_state_t *state, uint32_t value) {
  auto t = cpu::decoder::rt(state);

  if (load) {
    if (state->is_load_delay_slot && state->load_index == t) {
      state->regs.gp[t] = state->load_value;
    }

    state->is_load = true;
    state->load_index = t;
    state->load_value = state->regs.gp[t];
  }

  state->regs.gp[t] = value;
  state->regs.gp[0] = 0;
}

template<bool forwarded = false>
static inline uint32_t get_rt(cpu_state_t *state) {
  return get_register<forwarded>(state, cpu::decoder::rt(state));
}

static inline uint32_t get_rs(cpu_state_t *state) {
  return get_register(state, cpu::decoder::rs(state));
}

// --============--
//   Instructions
// --============--

void cpu::op_add(cpu_state_t *state) {
  auto x = get_rs(state);
  auto y = get_rt(state);
  auto z = x + y;

  if (overflow(x, y, z)) {
    cop0::enter_exception(state, 0xc);
  }
  else {
    set_rd(state, z);
  }
}

void cpu::op_addi(cpu_state_t *state) {
  auto x = get_rs(state);
  auto y = decoder::iconst(state);
  auto z = x + y;

  if (overflow(x, y, z)) {
    cop0::enter_exception(state, 0xc);
  }
  else {
    set_rt(state, z);
  }
}

void cpu::op_addiu(cpu_state_t *state) {
  set_rt(state, get_rs(state) + decoder::iconst(state));
}

void cpu::op_addu(cpu_state_t *state) {
  set_rd(state, get_rs(state) + get_rt(state));
}

void cpu::op_and(cpu_state_t *state) {
  set_rd(state, get_rs(state) & get_rt(state));
}

void cpu::op_andi(cpu_state_t *state) {
  set_rt(state, get_rs(state) & decoder::uconst(state));
}

void cpu::op_beq(cpu_state_t *state) {
  if (get_rs(state) == get_rt(state)) {
    state->regs.next_pc = state->regs.pc + (decoder::iconst(state) << 2);
    state->is_branch = true;
  }
}

void cpu::op_bgtz(cpu_state_t *state) {
  if (int32_t(get_rs(state)) > 0) {
    state->regs.next_pc = state->regs.pc + (decoder::iconst(state) << 2);
    state->is_branch = true;
  }
}

void cpu::op_blez(cpu_state_t *state) {
  if (int32_t(get_rs(state)) <= 0) {
    state->regs.next_pc = state->regs.pc + (decoder::iconst(state) << 2);
    state->is_branch = true;
  }
}

void cpu::op_bne(cpu_state_t *state) {
  if (get_rs(state) != get_rt(state)) {
    state->regs.next_pc = state->regs.pc + (decoder::iconst(state) << 2);
    state->is_branch = true;
  }
}

void cpu::op_break(cpu_state_t *state) {
  cop0::enter_exception(state, 0x09);
}

void cpu::op_bxx(cpu_state_t *state) {
  // bgez rs,$nnnn
  // bgezal rs,$nnnn
  // bltz rs,$nnnn
  // bltzal rs,$nnnn
  bool condition = (state->code & (1 << 16))
    ? int32_t(get_rs(state)) >= 0
    : int32_t(get_rs(state)) <  0;

  if ((state->code & 0x1e0000) == 0x100000) {
    state->regs.gp[31] = state->regs.next_pc;
  }

  if (condition) {
    state->regs.next_pc = state->regs.pc + (decoder::iconst(state) << 2);
    state->is_branch = true;
  }
}

void cpu::op_cop0(cpu_state_t *state) {
  if (state->code & (1 << 25)) {
    return cop0::run(state, state->code & 0x1ffffff);
  }

  auto rd = cpu::decoder::rd(state);
  auto rt = cpu::decoder::rt(state);

  switch (cpu::decoder::rs(state)) {
  case 0x00: return set_rt(state, cop0::read_gpr(state, rd));
  case 0x02: return set_rt(state, cop0::read_gpr(state, rd));
  case 0x04: return cop0::write_gpr(state, rd, get_register(state, rt));
  case 0x06: return cop0::write_ccr(state, rd, get_register(state, rt));
  }

  printf("cop0 $%08x\n", state->code);
}

void cpu::op_cop1(cpu_state_t *state) {
  cop0::enter_exception(state, 0xb);
}

void cpu::op_cop2(cpu_state_t *state) {
  if (state->code & (1 << 25)) {
    return cop2::run(&state->cop2, state->code & 0x1ffffff);
  }

  auto rd = cpu::decoder::rd(state);
  auto rt = cpu::decoder::rt(state);

  switch (cpu::decoder::rs(state)) {
  case 0x00: return set_rt(state, cop2::read_gpr(&state->cop2, rd));
  case 0x02: return set_rt(state, cop2::read_ccr(&state->cop2, rd));
  case 0x04: return cop2::write_gpr(&state->cop2, rd, get_register(state, rt));
  case 0x06: return cop2::write_ccr(&state->cop2, rd, get_register(state, rt));
  }

  printf("cop2 $%08x\n", state->code);
}

void cpu::op_cop3(cpu_state_t *state) {
  cop0::enter_exception(state, 0xb);
}

void cpu::op_div(cpu_state_t *state) {
  int32_t dividend = int32_t(get_rs(state));
  int32_t divisor = int32_t(get_rt(state));

  if (dividend == int32_t(0x80000000) && divisor == int32_t(0xffffffff)) {
    state->regs.lo = 0x80000000;
    state->regs.hi = 0;
  }
  else if (dividend >= 0 && divisor == 0) {
    state->regs.lo = uint32_t(0xffffffff);
    state->regs.hi = uint32_t(dividend);
  }
  else if (dividend <= 0 && divisor == 0) {
    state->regs.lo = uint32_t(0x00000001);
    state->regs.hi = uint32_t(dividend);
  }
  else {
    state->regs.lo = uint32_t(dividend / divisor);
    state->regs.hi = uint32_t(dividend % divisor);
  }
}

void cpu::op_divu(cpu_state_t *state) {
  auto dividend = get_rs(state);
  auto divisor = get_rt(state);

  if (divisor) {
    state->regs.lo = dividend / divisor;
    state->regs.hi = dividend % divisor;
  }
  else {
    state->regs.lo = 0xffffffff;
    state->regs.hi = dividend;
  }
}

void cpu::op_j(cpu_state_t *state) {
  state->regs.next_pc = (state->regs.pc & 0xf0000000) | ((state->code << 2) & 0x0ffffffc);
  state->is_branch = true;
}

void cpu::op_jal(cpu_state_t *state) {
  state->regs.gp[31] = state->regs.next_pc;
  state->regs.next_pc = (state->regs.pc & 0xf0000000) | ((state->code << 2) & 0x0ffffffc);
  state->is_branch = true;
}

void cpu::op_jalr(cpu_state_t *state) {
  auto ra = state->regs.next_pc;

  state->regs.next_pc = get_rs(state);
  set_rd(state, ra);

  state->is_branch = true;
}

void cpu::op_jr(cpu_state_t *state) {
  state->regs.next_pc = get_rs(state);
  state->is_branch = true;
}

void cpu::op_lb(cpu_state_t *state) {
  auto address = get_rs(state) + decoder::iconst(state);
  auto data = read_data(state, bus::BUS_WIDTH_BYTE, address);
  data = utility::sclip<8>(data);

  set_rt<1>(state, data);
}

void cpu::op_lbu(cpu_state_t *state) {
  auto address = get_rs(state) + decoder::iconst(state);
  auto data = read_data(state, bus::BUS_WIDTH_BYTE, address);

  set_rt<1>(state, data);
}

void cpu::op_lh(cpu_state_t *state) {
  auto address = get_rs(state) + decoder::iconst(state);
  if (address & 1) {
    cop0::enter_exception(state, 0x4);
  }
  else {
    auto data = read_data(state, bus::BUS_WIDTH_HALF, address);
    data = utility::sclip<16>(data);

    set_rt<1>(state, data);
  }
}

void cpu::op_lhu(cpu_state_t *state) {
  auto address = get_rs(state) + decoder::iconst(state);
  if (address & 1) {
    cop0::enter_exception(state, 0x4);
  }
  else {
    auto data = read_data(state, bus::BUS_WIDTH_HALF, address);

    set_rt<1>(state, data);
  }
}

void cpu::op_lui(cpu_state_t *state) {
  set_rt(state, decoder::uconst(state) << 16);
}

void cpu::op_lw(cpu_state_t *state) {
  auto address = get_rs(state) + decoder::iconst(state);
  if (address & 3) {
    cop0::enter_exception(state, 0x4);
  }
  else {
    auto data = read_data(state, bus::BUS_WIDTH_WORD, address);

    set_rt<1>(state, data);
  }
}

void cpu::op_lwc0(cpu_state_t *state) {
  throw "unimplemented lwc0\n";
}

void cpu::op_lwc1(cpu_state_t *state) {
  throw "unimplemented lwc1\n";
}

void cpu::op_lwc2(cpu_state_t *state) {
  auto address = get_rs(state) + decoder::iconst(state);
  if (address & 3) {
    cop0::enter_exception(state, 0x4);
  }
  else {
    auto data = read_data(state, bus::BUS_WIDTH_WORD, address);

    cop2::write_gpr(&state->cop2, decoder::rt(state), data);
  }
}

void cpu::op_lwc3(cpu_state_t *state) {
  throw "unimplemented lwc3\n";
}

void cpu::op_lwl(cpu_state_t *state) {
  auto address = get_rs(state) + decoder::iconst(state);
  auto data = read_data(state, bus::BUS_WIDTH_WORD, address & ~3);

  switch (address & 3) {
  default: data = (data << 24) | (get_rt<1>(state) & 0x00ffffff); break;
  case  1: data = (data << 16) | (get_rt<1>(state) & 0x0000ffff); break;
  case  2: data = (data <<  8) | (get_rt<1>(state) & 0x000000ff); break;
  case  3: data = (data <<  0) | (get_rt<1>(state) & 0x00000000); break;
  }

  set_rt<1>(state, data);
}

void cpu::op_lwr(cpu_state_t *state) {
  auto address = get_rs(state) + decoder::iconst(state);
  auto data = read_data(state, bus::BUS_WIDTH_WORD, address & ~3);

  switch (address & 3) {
  default: data = (data >>  0) | (get_rt<1>(state) & 0x00000000); break;
  case  1: data = (data >>  8) | (get_rt<1>(state) & 0xff000000); break;
  case  2: data = (data >> 16) | (get_rt<1>(state) & 0xffff0000); break;
  case  3: data = (data >> 24) | (get_rt<1>(state) & 0xffffff00); break;
  }

  set_rt<1>(state, data);
}

void cpu::op_mfhi(cpu_state_t *state) {
  set_rd(state, state->regs.hi);
}

void cpu::op_mflo(cpu_state_t *state) {
  set_rd(state, state->regs.lo);
}

void cpu::op_mthi(cpu_state_t *state) {
  state->regs.hi = get_rs(state);
}

void cpu::op_mtlo(cpu_state_t *state) {
  state->regs.lo = get_rs(state);
}

void cpu::op_mult(cpu_state_t *state) {
  auto s = int32_t(get_rs(state));
  auto t = int32_t(get_rt(state));

  int64_t result = int64_t(s) * int64_t(t);
  state->regs.lo = uint32_t(result >> 0);
  state->regs.hi = uint32_t(result >> 32);
}

void cpu::op_multu(cpu_state_t *state) {
  auto s = get_rs(state);
  auto t = get_rt(state);

  uint64_t result = uint64_t(s) * uint64_t(t);
  state->regs.lo = uint32_t(result >> 0);
  state->regs.hi = uint32_t(result >> 32);
}

void cpu::op_nor(cpu_state_t *state) {
  set_rd(state, ~(get_rs(state) | get_rt(state)));
}

void cpu::op_or(cpu_state_t *state) {
  set_rd(state, get_rs(state) | get_rt(state));
}

void cpu::op_ori(cpu_state_t *state) {
  set_rt(state, get_rs(state) | decoder::uconst(state));
}

void cpu::op_sb(cpu_state_t *state) {
  auto address = get_rs(state) + decoder::iconst(state);
  auto data = get_rt(state);

  write_data(state, bus::BUS_WIDTH_BYTE, address, data);
}

void cpu::op_sh(cpu_state_t *state) {
  auto address = get_rs(state) + decoder::iconst(state);
  if (address & 1) {
    cop0::enter_exception(state, 0x5);
  }
  else {
    auto data = get_rt(state);

    write_data(state, bus::BUS_WIDTH_HALF, address, data);
  }
}

void cpu::op_sll(cpu_state_t *state) {
  set_rd(state, get_rt(state) << decoder::sa(state));
}

void cpu::op_sllv(cpu_state_t *state) {
  set_rd(state, get_rt(state) << get_rs(state));
}

void cpu::op_slt(cpu_state_t *state) {
  set_rd(state, int32_t(get_rs(state)) < int32_t(get_rt(state)) ? 1 : 0);
}

void cpu::op_slti(cpu_state_t *state) {
  set_rt(state, int32_t(get_rs(state)) < int32_t(decoder::iconst(state)) ? 1 : 0);
}

void cpu::op_sltiu(cpu_state_t *state) {
  set_rt(state, get_rs(state) < decoder::iconst(state) ? 1 : 0);
}

void cpu::op_sltu(cpu_state_t *state) {
  set_rd(state, get_rs(state) < get_rt(state) ? 1 : 0);
}

void cpu::op_sra(cpu_state_t *state) {
  set_rd(state, int32_t(get_rt(state)) >> decoder::sa(state));
}

void cpu::op_srav(cpu_state_t *state) {
  set_rd(state, int32_t(get_rt(state)) >> get_rs(state));
}

void cpu::op_srl(cpu_state_t *state) {
  set_rd(state, get_rt(state) >> decoder::sa(state));
}

void cpu::op_srlv(cpu_state_t *state) {
  set_rd(state, get_rt(state) >> get_rs(state));
}

void cpu::op_sub(cpu_state_t *state) {
  auto x = get_rs(state);
  auto y = get_rt(state);
  auto z = x - y;

  if (overflow(x, ~y, z)) {
    cop0::enter_exception(state, 0xc);
  }
  else {
    set_rd(state, z);
  }
}

void cpu::op_subu(cpu_state_t *state) {
  set_rd(state, get_rs(state) - get_rt(state));
}

void cpu::op_sw(cpu_state_t *state) {
  auto address = get_rs(state) + decoder::iconst(state);
  if (address & 3) {
    cop0::enter_exception(state, 0x5);
  }
  else {
    auto data = get_rt(state);

    write_data(state, bus::BUS_WIDTH_WORD, address, data);
  }
}

void cpu::op_swc0(cpu_state_t *state) {
  throw "unimplemented swc0\n";
}

void cpu::op_swc1(cpu_state_t *state) {
  throw "unimplemented swc1\n";
}

void cpu::op_swc2(cpu_state_t *state) {
  auto address = get_rs(state) + decoder::iconst(state);
  if (address & 3) {
    cop0::enter_exception(state, 0x5);
  }
  else {
    auto data = cop2::read_gpr(&state->cop2, decoder::rt(state));

    write_data(state, bus::BUS_WIDTH_WORD, address, data);
  }
}

void cpu::op_swc3(cpu_state_t *state) {
  throw "unimplemented swc3\n";
}

void cpu::op_swl(cpu_state_t *state) {
  auto address = get_rs(state) + decoder::iconst(state);
  auto data = read_data(state, bus::BUS_WIDTH_WORD, address & ~3);

  switch (address & 3) {
  default: data = (data & 0xffffff00) | (get_rt(state) >> 24); break;
  case  1: data = (data & 0xffff0000) | (get_rt(state) >> 16); break;
  case  2: data = (data & 0xff000000) | (get_rt(state) >>  8); break;
  case  3: data = (data & 0x00000000) | (get_rt(state) >>  0); break;
  }

  write_data(state, bus::BUS_WIDTH_WORD, address & ~3, data);
}

void cpu::op_swr(cpu_state_t *state) {
  auto address = get_rs(state) + decoder::iconst(state);
  auto data = read_data(state, bus::BUS_WIDTH_WORD, address & ~3);

  switch (address & 3) {
  default: data = (data & 0x00000000) | (get_rt(state) <<  0); break;
  case  1: data = (data & 0x000000ff) | (get_rt(state) <<  8); break;
  case  2: data = (data & 0x0000ffff) | (get_rt(state) << 16); break;
  case  3: data = (data & 0x00ffffff) | (get_rt(state) << 24); break;
  }

  write_data(state, bus::BUS_WIDTH_WORD, address & ~3, data);
}

void cpu::op_syscall(cpu_state_t *state) {
  cop0::enter_exception(state, 0x08);
}

void cpu::op_xor(cpu_state_t *state) {
  set_rd(state, get_rs(state) ^ get_rt(state));
}

void cpu::op_xori(cpu_state_t *state) {
  set_rt(state, get_rs(state) ^ decoder::uconst(state));
}

void cpu::op_und(cpu_state_t *state) {
  cop0::enter_exception(state, 0xa);
}
