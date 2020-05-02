#include "cpu/core.hpp"

#include "cpu/cop0/sys.hpp"
#include "cpu/cop2/gte.hpp"
#include "util/int.hpp"
#include "util/uint.hpp"
#include "args.hpp"

using namespace psx::cpu;
using namespace psx::util;

core_t::opcode_t core_t::op_table[64] = {
  nullptr,          &core_t::op_bxx,   &core_t::op_j,    &core_t::op_jal,
  &core_t::op_beq,  &core_t::op_bne,   &core_t::op_blez, &core_t::op_bgtz,
  &core_t::op_addi, &core_t::op_addiu, &core_t::op_slti, &core_t::op_sltiu,
  &core_t::op_andi, &core_t::op_ori,   &core_t::op_xori, &core_t::op_lui,
  &core_t::op_cop0, &core_t::op_cop1,  &core_t::op_cop2, &core_t::op_cop3,
  &core_t::op_und,  &core_t::op_und,   &core_t::op_und,  &core_t::op_und,
  &core_t::op_und,  &core_t::op_und,   &core_t::op_und,  &core_t::op_und,
  &core_t::op_und,  &core_t::op_und,   &core_t::op_und,  &core_t::op_und,
  &core_t::op_lb,   &core_t::op_lh,    &core_t::op_lwl,  &core_t::op_lw,
  &core_t::op_lbu,  &core_t::op_lhu,   &core_t::op_lwr,  &core_t::op_und,
  &core_t::op_sb,   &core_t::op_sh,    &core_t::op_swl,  &core_t::op_sw,
  &core_t::op_und,  &core_t::op_und,   &core_t::op_swr,  &core_t::op_und,
  &core_t::op_lwc0, &core_t::op_lwc1,  &core_t::op_lwc2, &core_t::op_lwc3,
  &core_t::op_und,  &core_t::op_und,   &core_t::op_und,  &core_t::op_und,
  &core_t::op_swc0, &core_t::op_swc1,  &core_t::op_swc2, &core_t::op_swc3,
  &core_t::op_und,  &core_t::op_und,   &core_t::op_und,  &core_t::op_und
};

core_t::opcode_t core_t::op_table_special[64] = {
  &core_t::op_sll,     &core_t::op_und,   &core_t::op_srl,  &core_t::op_sra,
  &core_t::op_sllv,    &core_t::op_und,   &core_t::op_srlv, &core_t::op_srav,
  &core_t::op_jr,      &core_t::op_jalr,  &core_t::op_und,  &core_t::op_und,
  &core_t::op_syscall, &core_t::op_break, &core_t::op_und,  &core_t::op_und,
  &core_t::op_mfhi,    &core_t::op_mthi,  &core_t::op_mflo, &core_t::op_mtlo,
  &core_t::op_und,     &core_t::op_und,   &core_t::op_und,  &core_t::op_und,
  &core_t::op_mult,    &core_t::op_multu, &core_t::op_div,  &core_t::op_divu,
  &core_t::op_und,     &core_t::op_und,   &core_t::op_und,  &core_t::op_und,
  &core_t::op_add,     &core_t::op_addu,  &core_t::op_sub,  &core_t::op_subu,
  &core_t::op_and,     &core_t::op_or,    &core_t::op_xor,  &core_t::op_nor,
  &core_t::op_und,     &core_t::op_und,   &core_t::op_slt,  &core_t::op_sltu,
  &core_t::op_und,     &core_t::op_und,   &core_t::op_und,  &core_t::op_und,
  &core_t::op_und,     &core_t::op_und,   &core_t::op_und,  &core_t::op_und,
  &core_t::op_und,     &core_t::op_und,   &core_t::op_und,  &core_t::op_und,
  &core_t::op_und,     &core_t::op_und,   &core_t::op_und,  &core_t::op_und,
  &core_t::op_und,     &core_t::op_und,   &core_t::op_und,  &core_t::op_und
};

core_t::core_t(addressable_t &memory)
  : addressable_t("cpu", args::log_cpu)
  , memory(memory)
  , dcache("dcache") {
  regs.gp[0] = 0;
  regs.pc = 0xbfc00000;
  regs.next_pc = regs.pc + 4;

  cop[0] = new cop0::sys_t();
  cop[2] = new cop2::gte_t();

  cop[0]->write_gpr(12, 0x00000000);
}

cop_t *core_t::get_cop(int n) const {
  return cop[n];
}

bool core_t::get_cop_usable(int n) const {
  if (get_cop(n) == nullptr) {
    return false;
  }

  auto stat = get_cop(0)->read_gpr(12);
  auto mask = (1 << (28 + n));

  return (n == 0 && (stat & 0x10000000) == 0)
    ? (stat & 0x02) == 0
    : (stat & mask) != 0;
}

int core_t::tick() {
  read_code();

  is_branch_delay_slot = is_branch;
  is_branch = false;

  is_load_delay_slot = is_load;
  is_load = false;

  bool iec = (get_cop(0)->read_gpr(12) & 1) != 0;
  bool irq = (get_cop(0)->read_gpr(12) & get_cop(0)->read_gpr(13) & 0xff00) != 0;

  if (iec && irq) {
    enter_exception(cop0::exception_t::interrupt, 0);
  }
  else {
    uint32_t code = (get_code() >> 26) & 63;
    if (code) {
      (*this.*op_table[code])();
    } else {
      (*this.*op_table_special[get_code() & 63])();
    }
  }

  return 4;
}

static uint32_t segments[8] = {
  0x7fffffff,  // kuseg ($0000_0000 - $7fff_ffff)
  0x7fffffff,  //
  0x7fffffff,  //
  0x7fffffff,  //
  0x1fffffff,  // kseg0 ($8000_0000 - $9fff_ffff)
  0x1fffffff,  // kseg1 ($a000_0000 - $bfff_ffff)
  0xffffffff,  // kseg2 ($c000_0000 - $ffff_ffff)
  0xffffffff   //
};

static inline uint32_t get_segment(uint32_t address) {
  return address >> 29;
}

static inline uint32_t map_address(uint32_t address) {
  return address & segments[get_segment(address)];
}

void core_t::enter_exception(cop0::exception_t code, int cop) {
  auto cop0 = (cop0::sys_t *) get_cop(0);

  cop0->push_flags();
  cop0->put_cause_excode(code);
  cop0->put_cause_ce(cop);
  cop0->put_cause_bt(is_branch_delay_slot && is_branch_taken);
  cop0->put_cause_bd(is_branch_delay_slot);

  if (is_branch_delay_slot) {
    if (is_branch_taken) {
      cop0->put_tar(branch_target);
    } else {
      cop0->put_tar(regs.this_pc + 4);
    }

    cop0->put_epc(regs.this_pc - 4);
  } else {
    cop0->put_epc(regs.this_pc);
  }

  if (cop0->get_bev()) {
    set_pc(0xbfc00180);
  } else {
    set_pc(0x80000080);
  }
}

void core_t::read_code() {
  if (regs.pc & 3) {
    return enter_exception(cop0::exception_t::address_error_load, 0);
  }

  regs.this_pc = regs.pc;
  regs.pc = regs.next_pc;
  regs.next_pc += 4;

  // TODO: read i-cache

  code = memory.io_read_word(map_address(regs.this_pc));
}

io_target_t core_t::get_target(uint32_t address) const {
  uint32_t cop0r12 = get_cop(0)->read_gpr(12);
  if (cop0r12 & cop0::ISC) {
    if (cop0r12 & cop0::SWC) {
      log("i-cache isolated access ~%08x", address);
      return io_target_t::ICACHE;
    }
    else {
      log("d-cache isolated access ~%08x", address);
      return io_target_t::DCACHE;
    }
  }

  if ((address & 0x7ffffc00) == 0x1f800000 && get_segment(address) < KSEG1) {
    log("d-cache memory access ~%08x", address);
    return io_target_t::DCACHE;
  }

  return io_target_t::MEMORY;
}

uint32_t core_t::read_data_byte(uint32_t address) {
  address = map_address(address);

  switch (get_target(address)) {
    case io_target_t::ICACHE: return 0;
    case io_target_t::DCACHE: return dcache.io_read_byte(address);
    case io_target_t::MEMORY: return memory.io_read_byte(address);
  }

  return 0;
}

uint32_t core_t::read_data_half(uint32_t address) {
  address = map_address(address);

  switch (get_target(address)) {
    case io_target_t::ICACHE: return 0;
    case io_target_t::DCACHE: return dcache.io_read_half(address);
    case io_target_t::MEMORY: return memory.io_read_half(address);
  }

  return 0;
}

uint32_t core_t::read_data_word(uint32_t address) {
  address = map_address(address);

  switch (get_target(address)) {
    case io_target_t::ICACHE: return 0;
    case io_target_t::DCACHE: return dcache.io_read_word(address);
    case io_target_t::MEMORY: return memory.io_read_word(address);
  }

  return 0;
}

void core_t::write_data_byte(uint32_t address, uint32_t data) {
  address = map_address(address);

  switch (get_target(address)) {
    case io_target_t::ICACHE: break;
    case io_target_t::DCACHE: dcache.io_write_byte(address, data); break;
    case io_target_t::MEMORY: memory.io_write_byte(address, data); break;
  }
}

void core_t::write_data_half(uint32_t address, uint32_t data) {
  address = map_address(address);

  switch (get_target(address)) {
    case io_target_t::ICACHE: break;
    case io_target_t::DCACHE: dcache.io_write_half(address, data); break;
    case io_target_t::MEMORY: memory.io_write_half(address, data); break;
  }
}

void core_t::write_data_word(uint32_t address, uint32_t data) {
  address = map_address(address);

  switch (get_target(address)) {
    case io_target_t::ICACHE: break;
    case io_target_t::DCACHE: dcache.io_write_word(address, data); break;
    case io_target_t::MEMORY: memory.io_write_word(address, data); break;
  }
}

void core_t::update_irq(uint32_t stat, uint32_t mask) {
  log("update irq: stat=0x%08x, mask=0x%08x", stat, mask);

  auto cop0 = (cop0::sys_t *) get_cop(0);

  istat = stat;
  imask = mask;

  if (istat & imask) {
    cop0->put_cause_ip(1);
  } else {
    cop0->put_cause_ip(0);
  }
}

uint32_t core_t::get_imask() const {
  return imask;
}

void core_t::set_imask(uint32_t value) {
  update_irq(get_istat(), value);
}

uint32_t core_t::get_istat() const {
  return istat;
}

void core_t::set_istat(uint32_t value) {
  update_irq(value, get_imask());
}

uint16_t core_t::io_read_half(uint32_t address) {
  switch (address) {
    case 0x1f801070:
      return get_istat();

    case 0x1f801074:
      return get_imask();
  }

  return 0;
}

uint32_t core_t::io_read_word(uint32_t address) {
  return io_read_half(address);
}

void core_t::io_write_half(uint32_t address, uint16_t data) {
  switch (address) {
    case 0x1f801070:
      return set_istat(data & istat);

    case 0x1f801074:
      return set_imask(data & 0x7ff);
  }
}

void core_t::io_write_word(uint32_t address, uint32_t data) {
  io_write_half(address, data);
}

// --========--
//   Decoding
// --========--

static inline uint32_t overflow(uint32_t x, uint32_t y, uint32_t z) {
  return (~(x ^ y) & (x ^ z) & 0x80000000);
}

uint32_t core_t::get_code() const {
  return code;
}

uint32_t core_t::get_pc() const {
  return regs.pc;
}

uint32_t core_t::get_rt() const {
  return get_register(decode_rt());
}

uint32_t core_t::get_rt_forwarded() const {
  return get_register_forwarded(decode_rt());
}

uint32_t core_t::get_rs() const {
  return get_register(decode_rs());
}

uint32_t core_t::get_register(uint32_t index) const {
  if (is_load_delay_slot && load_index == index) {
    return load_value;
  }
  else {
    return regs.gp[index];
  }
}

uint32_t core_t::get_register_forwarded(uint32_t index) const {
  return regs.gp[index];
}

void core_t::set_pc(uint32_t value) {
  regs.this_pc = value;
  regs.pc = value;
  regs.next_pc = value + sizeof(uint32_t);
}

void core_t::set_rd(uint32_t value) {
  regs.gp[decode_rd()] = value;
  regs.gp[0] = 0;
}

void core_t::set_rt(uint32_t value) {
  regs.gp[decode_rt()] = value;
  regs.gp[0] = 0;
}

void core_t::set_rt_load(uint32_t value) {
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

void core_t::set_register(uint32_t index, uint32_t value) {
  regs.gp[index] = value;
}

void core_t::branch(uint32_t target, bool condition) {
  is_branch = true;
  is_branch_taken = condition;
  branch_target = target;

  if (condition) {
    regs.next_pc = target;
  }
}

uint32_t core_t::branch_abs() {
  return (regs.pc & 0xf0000000) | ((get_code() << 2) & 0x0ffffffc);
}

uint32_t core_t::branch_rel() {
  return regs.pc + (decode_iconst() << 2);
}

// --============--
//   Instructions
// --============--

void core_t::op_add() {
  uint32_t x = get_rs();
  uint32_t y = get_rt();
  uint32_t z = x + y;

  if (overflow(x, y, z)) {
    return enter_exception(cop0::exception_t::overflow, 0);
  }

  set_rd(z);
}

void core_t::op_addi() {
  uint32_t x = get_rs();
  uint32_t y = decode_iconst();
  uint32_t z = x + y;

  if (overflow(x, y, z)) {
    return enter_exception(cop0::exception_t::overflow, 0);
  }

  set_rt(z);
}

void core_t::op_addiu() {
  set_rt(get_rs() + decode_iconst());
}

void core_t::op_addu() {
  set_rd(get_rs() + get_rt());
}

void core_t::op_and() {
  set_rd(get_rs() & get_rt());
}

void core_t::op_andi() {
  set_rt(get_rs() & decode_uconst());
}

void core_t::op_beq() {
  branch(branch_rel(), get_rs() == get_rt());
}

void core_t::op_bgtz() {
  branch(branch_rel(), int32_t(get_rs()) > 0);
}

void core_t::op_blez() {
  branch(branch_rel(), int32_t(get_rs()) <= 0);
}

void core_t::op_bne() {
  branch(branch_rel(), get_rs() != get_rt());
}

void core_t::op_break() {
  enter_exception(cop0::exception_t::breakpoint, 0);
}

void core_t::op_bxx() {
  // bgez rs,$nnnn
  // bgezal rs,$nnnn
  // bltz rs,$nnnn
  // bltzal rs,$nnnn
  bool condition = (get_code() & (1 << 16))
    ? int32_t(get_rs()) >= 0
    : int32_t(get_rs()) <  0;

  if ((get_code() & 0x1e0000) == 0x100000) {
    regs.gp[31] = regs.next_pc;
  }

  branch(branch_rel(), condition);
}

void core_t::op_cop(int n) {
  if (get_cop_usable(n) == false) {
    return enter_exception(cop0::exception_t::cop_unusable, n);
  }

  auto cop = get_cop(n);

  if (get_code() & (1 << 25)) {
    return cop->run(get_code() & 0x1ffffff);
  }

  uint32_t rd = decode_rd();
  uint32_t rt = decode_rt();

  switch (decode_rs()) {
    case 0x00: return set_rt(cop->read_gpr(rd));
    case 0x02: return set_rt(cop->read_ccr(rd));
    case 0x04: return cop->write_gpr(rd, get_register(rt));
    case 0x06: return cop->write_ccr(rd, get_register(rt));
  }

  log("op_cop%d(0x%08x)", n, get_code());
}

void core_t::op_cop0() {
  op_cop(0);
}

void core_t::op_cop1() {
  op_cop(1);
}

void core_t::op_cop2() {
  op_cop(2);
}

void core_t::op_cop3() {
  op_cop(3);
}

void core_t::op_div() {
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

void core_t::op_divu() {
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

void core_t::op_j() {
  branch(branch_abs(), true);
}

void core_t::op_jal() {
  regs.gp[31] = regs.next_pc;
  branch(branch_abs(), true);
}

void core_t::op_jalr() {
  uint32_t ra = regs.next_pc;
  branch(get_rs(), true);

  set_rd(ra);
}

void core_t::op_jr() {
  branch(get_rs(), true);
}

void core_t::op_lb() {
  uint32_t address = get_rs() + decode_iconst();
  uint32_t data = read_data_byte(address);
  data = int_t<8>::trunc(data);

  set_rt_load(data);
}

void core_t::op_lbu() {
  uint32_t address = get_rs() + decode_iconst();
  uint32_t data = read_data_byte(address);
  data = uint_t<8>::trunc(data);

  set_rt_load(data);
}

void core_t::op_lh() {
  uint32_t address = get_rs() + decode_iconst();
  if (address & 1) {
    return enter_exception(cop0::exception_t::address_error_load, 0);
  }

  uint32_t data = read_data_half(address);
  data = int_t<16>::trunc(data);

  set_rt_load(data);
}

void core_t::op_lhu() {
  uint32_t address = get_rs() + decode_iconst();
  if (address & 1) {
    return enter_exception(cop0::exception_t::address_error_load, 0);
  }

  uint32_t data = read_data_half(address);
  data = uint_t<16>::trunc(data);

  set_rt_load(data);
}

void core_t::op_lui() {
  set_rt(decode_uconst() << 16);
}

void core_t::op_lw() {
  uint32_t address = get_rs() + decode_iconst();
  if (address & 3) {
    return enter_exception(cop0::exception_t::address_error_load, 0);
  }

  uint32_t data = read_data_word(address);

  set_rt_load(data);
}

void core_t::op_lwc(int n) {
  if (get_cop_usable(n) == false) {
    return enter_exception(cop0::exception_t::cop_unusable, n);
  }

  uint32_t address = get_rs() + decode_iconst();
  if (address & 3) {
    return enter_exception(cop0::exception_t::address_error_load, 0);
  }

  get_cop(n)->write_gpr(decode_rt(), read_data_word(address));
}

void core_t::op_lwc0() {
  op_lwc(0);
}

void core_t::op_lwc1() {
  op_lwc(1);
}

void core_t::op_lwc2() {
  op_lwc(2);
}

void core_t::op_lwc3() {
  op_lwc(3);
}

void core_t::op_lwl() {
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

void core_t::op_lwr() {
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

void core_t::op_mfhi() {
  set_rd(regs.hi);
}

void core_t::op_mflo() {
  set_rd(regs.lo);
}

void core_t::op_mthi() {
  regs.hi = get_rs();
}

void core_t::op_mtlo() {
  regs.lo = get_rs();
}

void core_t::op_mult() {
  int32_t rs = int32_t(get_rs());
  int32_t rt = int32_t(get_rt());

  int64_t result = int64_t(rs) * int64_t(rt);
  regs.lo = uint32_t(result >> 0);
  regs.hi = uint32_t(result >> 32);
}

void core_t::op_multu() {
  uint32_t s = get_rs();
  uint32_t t = get_rt();

  uint64_t result = uint64_t(s) * uint64_t(t);
  regs.lo = uint32_t(result >> 0);
  regs.hi = uint32_t(result >> 32);
}

void core_t::op_nor() {
  set_rd(~(get_rs() | get_rt()));
}

void core_t::op_or() {
  set_rd(get_rs() | get_rt());
}

void core_t::op_ori() {
  set_rt(get_rs() | decode_uconst());
}

void core_t::op_sb() {
  uint32_t address = get_rs() + decode_iconst();
  uint32_t data = get_rt();

  write_data_byte(address, data);
}

void core_t::op_sh() {
  uint32_t address = get_rs() + decode_iconst();
  if (address & 1) {
    return enter_exception(cop0::exception_t::address_error_store, 0);
  }

  write_data_half(address, get_rt());
}

void core_t::op_sll() {
  set_rd(get_rt() << decode_sa());
}

void core_t::op_sllv() {
  set_rd(get_rt() << get_rs());
}

void core_t::op_slt() {
  set_rd(int32_t(get_rs()) < int32_t(get_rt()) ? 1 : 0);
}

void core_t::op_slti() {
  set_rt(int32_t(get_rs()) < int32_t(decode_iconst()) ? 1 : 0);
}

void core_t::op_sltiu() {
  set_rt(get_rs() < decode_iconst() ? 1 : 0);
}

void core_t::op_sltu() {
  set_rd(get_rs() < get_rt() ? 1 : 0);
}

void core_t::op_sra() {
  set_rd(int32_t(get_rt()) >> decode_sa());
}

void core_t::op_srav() {
  set_rd(int32_t(get_rt()) >> get_rs());
}

void core_t::op_srl() {
  set_rd(get_rt() >> decode_sa());
}

void core_t::op_srlv() {
  set_rd(get_rt() >> get_rs());
}

void core_t::op_sub() {
  uint32_t x = get_rs();
  uint32_t y = get_rt();
  uint32_t z = x - y;

  if (overflow(x, ~y, z)) {
    return enter_exception(cop0::exception_t::overflow, 0);
  }

  set_rd(z);
}

void core_t::op_subu() {
  set_rd(get_rs() - get_rt());
}

void core_t::op_sw() {
  uint32_t address = get_rs() + decode_iconst();
  if (address & 3) {
    return enter_exception(cop0::exception_t::address_error_store, 0);
  }

  uint32_t data = get_rt();

  write_data_word(address, data);
}

void core_t::op_swc(int n) {
  if (get_cop_usable(n) == false) {
    return enter_exception(cop0::exception_t::cop_unusable, n);
  }

  uint32_t address = get_rs() + decode_iconst();
  if (address & 3) {
    return enter_exception(cop0::exception_t::address_error_store, 0);
  }

  write_data_word(address, get_cop(n)->read_gpr(decode_rt()));
}

void core_t::op_swc0() {
  op_swc(0);
}

void core_t::op_swc1() {
  op_swc(1);
}

void core_t::op_swc2() {
  op_swc(2);
}

void core_t::op_swc3() {
  op_swc(3);
}

void core_t::op_swl() {
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

void core_t::op_swr() {
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

void core_t::op_syscall() {
  enter_exception(cop0::exception_t::syscall, 0);
}

void core_t::op_xor() {
  set_rd(get_rs() ^ get_rt());
}

void core_t::op_xori() {
  set_rt(get_rs() ^ decode_uconst());
}

void core_t::op_und() {
  enter_exception(cop0::exception_t::reserved_instruction, 0);
}
