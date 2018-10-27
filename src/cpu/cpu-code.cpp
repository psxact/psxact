#include "cpu/cpu.hpp"

#include "cpu/cpu-cop0.hpp"
#include "cpu/cpu-cop2.hpp"
#include "utility.hpp"


using namespace psx;

// --========--
//   Decoding
// --========--


static inline uint32_t overflow(uint32_t x, uint32_t y, uint32_t z) {
  return (~(x ^ y) & (x ^ z) & 0x80000000);
}


uint32_t cpu_t::get_register(uint32_t index) {
  if (is_load_delay_slot && load_index == index) {
    return load_value;
  }
  else {
    return regs.gp[index];
  }
}


uint32_t cpu_t::get_register_forwarded(uint32_t index) {
  return regs.gp[index];
}


void cpu_t::set_rd(uint32_t value) {
  regs.gp[decode_rd()] = value;
  regs.gp[0] = 0;
}


void cpu_t::set_rt(uint32_t value) {
  regs.gp[decode_rt()] = value;
  regs.gp[0] = 0;
}


void cpu_t::set_rt_load(uint32_t value) {
  uint32_t t = decode_rt();

  if (is_load_delay_slot && load_index == t) {
    regs.gp[t] = load_value;
  }

  is_load = true;
  load_index = t;
  load_value = regs.gp[t];

  regs.gp[t] = value;
  regs.gp[0] = 0;
}


uint32_t cpu_t::get_rt() {
  return get_register(decode_rt());
}


uint32_t cpu_t::get_rt_forwarded() {
  return get_register_forwarded(decode_rt());
}


uint32_t cpu_t::get_rs() {
  return get_register(decode_rs());
}


// --============--
//   Instructions
// --============--


void cpu_t::op_add() {
  uint32_t x = get_rs();
  uint32_t y = get_rt();
  uint32_t z = x + y;

  if (overflow(x, y, z)) {
    return enter_exception(cop0_exception_code_t::overflow);
  }

  set_rd(z);
}


void cpu_t::op_addi() {
  uint32_t x = get_rs();
  uint32_t y = decode_iconst();
  uint32_t z = x + y;

  if (overflow(x, y, z)) {
    return enter_exception(cop0_exception_code_t::overflow);
  }

  set_rt(z);
}


void cpu_t::op_addiu() {
  set_rt(get_rs() + decode_iconst());
}


void cpu_t::op_addu() {
  set_rd(get_rs() + get_rt());
}


void cpu_t::op_and() {
  set_rd(get_rs() & get_rt());
}


void cpu_t::op_andi() {
  set_rt(get_rs() & decode_uconst());
}


void cpu_t::op_beq() {
  if (get_rs() == get_rt()) {
    regs.next_pc = regs.pc + (decode_iconst() << 2);
    is_branch = true;
  }
}


void cpu_t::op_bgtz() {
  if (int32_t(get_rs()) > 0) {
    regs.next_pc = regs.pc + (decode_iconst() << 2);
    is_branch = true;
  }
}


void cpu_t::op_blez() {
  if (int32_t(get_rs()) <= 0) {
    regs.next_pc = regs.pc + (decode_iconst() << 2);
    is_branch = true;
  }
}


void cpu_t::op_bne() {
  if (get_rs() != get_rt()) {
    regs.next_pc = regs.pc + (decode_iconst() << 2);
    is_branch = true;
  }
}


void cpu_t::op_break() {
  enter_exception(cop0_exception_code_t::breakpoint);
}


void cpu_t::op_bxx() {
  // bgez rs,$nnnn
  // bgezal rs,$nnnn
  // bltz rs,$nnnn
  // bltzal rs,$nnnn
  bool condition = (code & (1 << 16))
    ? int32_t(get_rs()) >= 0
    : int32_t(get_rs()) <  0
    ;

  if ((code & 0x1e0000) == 0x100000) {
    regs.gp[31] = regs.next_pc;
  }

  if (condition) {
    regs.next_pc = regs.pc + (decode_iconst() << 2);
    is_branch = true;
  }
}


void cpu_t::op_cop(int n) {
  if (get_cop_usable(n) == false) {
    return enter_exception(cop0_exception_code_t::cop_unusable);
  }

  auto cop = get_cop(n);

  if (code & (1 << 25)) {
    return cop->run(code & 0x1ffffff);
  }

  uint32_t rd = decode_rd();
  uint32_t rt = decode_rt();

  switch (decode_rs()) {
    case 0x00: return set_rt(cop->read_gpr(rd));
    case 0x02: return set_rt(cop->read_ccr(rd));
    case 0x04: return cop->write_gpr(rd, get_register(rt));
    case 0x06: return cop->write_ccr(rd, get_register(rt));
  }

  printf("[cpu] op_cop%d(0x%08x)\n", n, code);
}


void cpu_t::op_cop0() {
  op_cop(0);
}


void cpu_t::op_cop1() {
  op_cop(1);
}


void cpu_t::op_cop2() {
  op_cop(2);
}


void cpu_t::op_cop3() {
  op_cop(3);
}


void cpu_t::op_div() {
  int32_t dividend = int32_t(get_rs());
  int32_t divisor = int32_t(get_rt());

  if (dividend == int32_t(0x80000000) && divisor == int32_t(0xffffffff)) {
    regs.lo = 0x80000000;
    regs.hi = 0;
  }
  else if (dividend >= 0 && divisor == 0) {
    regs.lo = uint32_t(0xffffffff);
    regs.hi = uint32_t(dividend);
  }
  else if (dividend <= 0 && divisor == 0) {
    regs.lo = uint32_t(0x00000001);
    regs.hi = uint32_t(dividend);
  }
  else {
    regs.lo = uint32_t(dividend / divisor);
    regs.hi = uint32_t(dividend % divisor);
  }
}


void cpu_t::op_divu() {
  uint32_t dividend = get_rs();
  uint32_t divisor = get_rt();

  if (divisor) {
    regs.lo = dividend / divisor;
    regs.hi = dividend % divisor;
  }
  else {
    regs.lo = 0xffffffff;
    regs.hi = dividend;
  }
}


void cpu_t::op_j() {
  regs.next_pc = (regs.pc & 0xf0000000) | ((code << 2) & 0x0ffffffc);
  is_branch = true;
}


void cpu_t::op_jal() {
  regs.gp[31] = regs.next_pc;
  regs.next_pc = (regs.pc & 0xf0000000) | ((code << 2) & 0x0ffffffc);
  is_branch = true;
}


void cpu_t::op_jalr() {
  uint32_t ra = regs.next_pc;

  regs.next_pc = get_rs();
  set_rd(ra);

  is_branch = true;
}


void cpu_t::op_jr() {
  regs.next_pc = get_rs();
  is_branch = true;
}


void cpu_t::op_lb() {
  uint32_t address = get_rs() + decode_iconst();
  uint32_t data = read_data_byte(address);
  data = utility::sclip<8>(data);

  set_rt_load(data);
}


void cpu_t::op_lbu() {
  uint32_t address = get_rs() + decode_iconst();
  uint32_t data = read_data_byte(address);
  data = utility::uclip<8>(data);

  set_rt_load(data);
}


void cpu_t::op_lh() {
  uint32_t address = get_rs() + decode_iconst();
  if (address & 1) {
    return enter_exception(cop0_exception_code_t::address_error_load);
  }

  uint32_t data = read_data_half(address);
  data = utility::sclip<16>(data);

  set_rt_load(data);
}


void cpu_t::op_lhu() {
  uint32_t address = get_rs() + decode_iconst();
  if (address & 1) {
    return enter_exception(cop0_exception_code_t::address_error_load);
  }

  uint32_t data = read_data_half(address);
  data = utility::uclip<16>(data);

  set_rt_load(data);
}


void cpu_t::op_lui() {
  set_rt(decode_uconst() << 16);
}


void cpu_t::op_lw() {
  uint32_t address = get_rs() + decode_iconst();
  if (address & 3) {
    return enter_exception(cop0_exception_code_t::address_error_load);
  }

  uint32_t data = read_data_word(address);

  set_rt_load(data);
}


void cpu_t::op_lwc(int n) {
  if (get_cop_usable(n) == false) {
    return enter_exception(cop0_exception_code_t::cop_unusable);
  }

  uint32_t address = get_rs() + decode_iconst();
  if (address & 3) {
    return enter_exception(cop0_exception_code_t::address_error_load);
  }

  get_cop(n)->write_gpr(decode_rt(), read_data_word(address));
}


void cpu_t::op_lwc0() {
  op_lwc(0);
}


void cpu_t::op_lwc1() {
  op_lwc(1);
}


void cpu_t::op_lwc2() {
  op_lwc(2);
}


void cpu_t::op_lwc3() {
  op_lwc(3);
}


void cpu_t::op_lwl() {
  uint32_t address = get_rs() + decode_iconst();
  uint32_t data = read_data_word(address & ~3);

  switch (address & 3) {
    case 0: data = (data << 24) | (get_rt_forwarded() & 0x00ffffff); break;
    case 1: data = (data << 16) | (get_rt_forwarded() & 0x0000ffff); break;
    case 2: data = (data <<  8) | (get_rt_forwarded() & 0x000000ff); break;
    case 3: data = (data <<  0) | (get_rt_forwarded() & 0x00000000); break;
  }

  set_rt_load(data);
}


void cpu_t::op_lwr() {
  uint32_t address = get_rs() + decode_iconst();
  uint32_t data = read_data_word(address & ~3);

  switch (address & 3) {
    case 0: data = (data >>  0) | (get_rt_forwarded() & 0x00000000); break;
    case 1: data = (data >>  8) | (get_rt_forwarded() & 0xff000000); break;
    case 2: data = (data >> 16) | (get_rt_forwarded() & 0xffff0000); break;
    case 3: data = (data >> 24) | (get_rt_forwarded() & 0xffffff00); break;
  }

  set_rt_load(data);
}


void cpu_t::op_mfhi() {
  set_rd(regs.hi);
}


void cpu_t::op_mflo() {
  set_rd(regs.lo);
}


void cpu_t::op_mthi() {
  regs.hi = get_rs();
}


void cpu_t::op_mtlo() {
  regs.lo = get_rs();
}


void cpu_t::op_mult() {
  int32_t rs = int32_t(get_rs());
  int32_t rt = int32_t(get_rt());

  int64_t result = int64_t(rs) * int64_t(rt);
  regs.lo = uint32_t(result >> 0);
  regs.hi = uint32_t(result >> 32);
}


void cpu_t::op_multu() {
  uint32_t s = get_rs();
  uint32_t t = get_rt();

  uint64_t result = uint64_t(s) * uint64_t(t);
  regs.lo = uint32_t(result >> 0);
  regs.hi = uint32_t(result >> 32);
}


void cpu_t::op_nor() {
  set_rd(~(get_rs() | get_rt()));
}


void cpu_t::op_or() {
  set_rd(get_rs() | get_rt());
}


void cpu_t::op_ori() {
  set_rt(get_rs() | decode_uconst());
}


void cpu_t::op_sb() {
  uint32_t address = get_rs() + decode_iconst();
  uint32_t data = get_rt();

  write_data_byte(address, data);
}


void cpu_t::op_sh() {
  uint32_t address = get_rs() + decode_iconst();
  if (address & 1) {
    return enter_exception(cop0_exception_code_t::address_error_store);
  }

  write_data_half(address, get_rt());
}


void cpu_t::op_sll() {
  set_rd(get_rt() << decode_sa());
}


void cpu_t::op_sllv() {
  set_rd(get_rt() << get_rs());
}


void cpu_t::op_slt() {
  set_rd(int32_t(get_rs()) < int32_t(get_rt()) ? 1 : 0);
}


void cpu_t::op_slti() {
  set_rt(int32_t(get_rs()) < int32_t(decode_iconst()) ? 1 : 0);
}


void cpu_t::op_sltiu() {
  set_rt(get_rs() < decode_iconst() ? 1 : 0);
}


void cpu_t::op_sltu() {
  set_rd(get_rs() < get_rt() ? 1 : 0);
}


void cpu_t::op_sra() {
  set_rd(int32_t(get_rt()) >> decode_sa());
}


void cpu_t::op_srav() {
  set_rd(int32_t(get_rt()) >> get_rs());
}


void cpu_t::op_srl() {
  set_rd(get_rt() >> decode_sa());
}


void cpu_t::op_srlv() {
  set_rd(get_rt() >> get_rs());
}


void cpu_t::op_sub() {
  uint32_t x = get_rs();
  uint32_t y = get_rt();
  uint32_t z = x - y;

  if (overflow(x, ~y, z)) {
    return enter_exception(cop0_exception_code_t::overflow);
  }

  set_rd(z);
}


void cpu_t::op_subu() {
  set_rd(get_rs() - get_rt());
}


void cpu_t::op_sw() {
  uint32_t address = get_rs() + decode_iconst();
  if (address & 3) {
    return enter_exception(cop0_exception_code_t::address_error_store);
  }

  uint32_t data = get_rt();

  write_data_word(address, data);
}


void cpu_t::op_swc(int n) {
  if (get_cop_usable(n) == false) {
    return enter_exception(cop0_exception_code_t::cop_unusable);
  }

  uint32_t address = get_rs() + decode_iconst();
  if (address & 3) {
    return enter_exception(cop0_exception_code_t::address_error_store);
  }

  write_data_word(address, get_cop(n)->read_gpr(decode_rt()));
}


void cpu_t::op_swc0() {
  op_swc(0);
}


void cpu_t::op_swc1() {
  op_swc(1);
}


void cpu_t::op_swc2() {
  op_swc(2);
}


void cpu_t::op_swc3() {
  op_swc(3);
}


void cpu_t::op_swl() {
  uint32_t address = get_rs() + decode_iconst();
  uint32_t data = read_data_word(address & ~3);

  switch (address & 3) {
    case 0: data = (data & 0xffffff00) | (get_rt() >> 24); break;
    case 1: data = (data & 0xffff0000) | (get_rt() >> 16); break;
    case 2: data = (data & 0xff000000) | (get_rt() >>  8); break;
    case 3: data = (data & 0x00000000) | (get_rt() >>  0); break;
  }

  write_data_word(address & ~3, data);
}


void cpu_t::op_swr() {
  uint32_t address = get_rs() + decode_iconst();
  uint32_t data = read_data_word(address & ~3);

  switch (address & 3) {
    case 0: data = (data & 0x00000000) | (get_rt() <<  0); break;
    case 1: data = (data & 0x000000ff) | (get_rt() <<  8); break;
    case 2: data = (data & 0x0000ffff) | (get_rt() << 16); break;
    case 3: data = (data & 0x00ffffff) | (get_rt() << 24); break;
  }

  write_data_word(address & ~3, data);
}


void cpu_t::op_syscall() {
  enter_exception(cop0_exception_code_t::syscall);
}


void cpu_t::op_xor() {
  set_rd(get_rs() ^ get_rt());
}


void cpu_t::op_xori() {
  set_rt(get_rs() ^ decode_uconst());
}


void cpu_t::op_und() {
  enter_exception(cop0_exception_code_t::reserved_instruction);
}
