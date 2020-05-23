#include "cpu/core.hpp"

#include "cpu/cop0/sys.hpp"
#include "cpu/cop2/gte.hpp"
#include "cpu/segment.hpp"
#include "util/int.hpp"
#include "util/uint.hpp"
#include "args.hpp"
#include "timing.hpp"

using namespace psx;
using namespace psx::cpu;
using namespace psx::util;

core::opcode core::op_table[64] = {
  nullptr,          &core::op_bxx,   &core::op_j,    &core::op_jal,
  &core::op_beq,  &core::op_bne,   &core::op_blez, &core::op_bgtz,
  &core::op_addi, &core::op_addiu, &core::op_slti, &core::op_sltiu,
  &core::op_andi, &core::op_ori,   &core::op_xori, &core::op_lui,
  &core::op_cop0, &core::op_cop1,  &core::op_cop2, &core::op_cop3,
  &core::op_und,  &core::op_und,   &core::op_und,  &core::op_und,
  &core::op_und,  &core::op_und,   &core::op_und,  &core::op_und,
  &core::op_und,  &core::op_und,   &core::op_und,  &core::op_und,
  &core::op_lb,   &core::op_lh,    &core::op_lwl,  &core::op_lw,
  &core::op_lbu,  &core::op_lhu,   &core::op_lwr,  &core::op_und,
  &core::op_sb,   &core::op_sh,    &core::op_swl,  &core::op_sw,
  &core::op_und,  &core::op_und,   &core::op_swr,  &core::op_und,
  &core::op_lwc0, &core::op_lwc1,  &core::op_lwc2, &core::op_lwc3,
  &core::op_und,  &core::op_und,   &core::op_und,  &core::op_und,
  &core::op_swc0, &core::op_swc1,  &core::op_swc2, &core::op_swc3,
  &core::op_und,  &core::op_und,   &core::op_und,  &core::op_und
};

core::opcode core::op_table_special[64] = {
  &core::op_sll,     &core::op_und,   &core::op_srl,  &core::op_sra,
  &core::op_sllv,    &core::op_und,   &core::op_srlv, &core::op_srav,
  &core::op_jr,      &core::op_jalr,  &core::op_und,  &core::op_und,
  &core::op_syscall, &core::op_break, &core::op_und,  &core::op_und,
  &core::op_mfhi,    &core::op_mthi,  &core::op_mflo, &core::op_mtlo,
  &core::op_und,     &core::op_und,   &core::op_und,  &core::op_und,
  &core::op_mult,    &core::op_multu, &core::op_div,  &core::op_divu,
  &core::op_und,     &core::op_und,   &core::op_und,  &core::op_und,
  &core::op_add,     &core::op_addu,  &core::op_sub,  &core::op_subu,
  &core::op_and,     &core::op_or,    &core::op_xor,  &core::op_nor,
  &core::op_und,     &core::op_und,   &core::op_slt,  &core::op_sltu,
  &core::op_und,     &core::op_und,   &core::op_und,  &core::op_und,
  &core::op_und,     &core::op_und,   &core::op_und,  &core::op_und,
  &core::op_und,     &core::op_und,   &core::op_und,  &core::op_und,
  &core::op_und,     &core::op_und,   &core::op_und,  &core::op_und,
  &core::op_und,     &core::op_und,   &core::op_und,  &core::op_und
};

core::core(addressable &memory)
  : addressable("cpu", args::log_cpu)
  , memory(memory)
  , dcache("dcache") {
  regs.pc = 0xbfc00000;
  regs.next_pc = regs.pc + 4;

  cop[0] = new cop0::sys();
  cop[2] = new cop2::gte();

  cop[0]->write_gpr(12, 0x00000000);
}

void core::interrupt(interrupt_type type) {
  int istat = get_istat() | static_cast<int>(type);
  set_istat(istat);
}

coprocessor *core::get_cop(int n) const {
  return cop[n];
}

bool core::get_cop_usable(int n) const {
  if (get_cop(n) == nullptr) {
    return false;
  }

  auto stat = get_cop(0)->read_gpr(12);
  auto mask = (1 << (28 + n));

  return (n == 0 && (stat & 0x10000000) == 0)
    ? (stat & 0x02) == 0
    : (stat & mask) != 0;
}

int core::tick() {
  timing::reset_cpu_time();

  read_code();

  is_branch_delay_slot = is_branch;
  is_branch = false;

  is_load_delay_slot = is_load;
  is_load = false;

  bool iec = (get_cop(0)->read_gpr(12) & 1) != 0;
  bool irq = (get_cop(0)->read_gpr(12) & get_cop(0)->read_gpr(13) & 0xff00) != 0;
  bool gte = (code & 0xfe000000) == 0x4a000000;

  if (iec && irq && !gte) {
    enter_exception(cop0::exception::interrupt, 0);
  } else {
    uint32_t code = (get_code() >> 26) & 63;
    if (code) {
      (*this.*op_table[code])();
    } else {
      (*this.*op_table_special[get_code() & 63])();
    }
  }

  return timing::get_cpu_time();
}

static inline uint32_t map_address(uint32_t address) {
  return address & get_segment_mask(address);
}

void core::enter_exception(cop0::exception code, int cop) {
  auto cop0 = (cop0::sys *) get_cop(0);

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

void core::read_code() {
  if (regs.pc & 3) {
    return enter_exception(cop0::exception::address_error_load, 0);
  }

  regs.this_pc = regs.pc;
  regs.pc = regs.next_pc;
  regs.next_pc += 4;

  // TODO: read i-cache

  code = memory.io_read(address_width::word, map_address(regs.this_pc));
}

io_target core::get_target(uint32_t address) const {
  uint32_t cop0r12 = get_cop(0)->read_gpr(12);
  if (cop0r12 & cop0::ISC) {
    if (cop0r12 & cop0::SWC) {
      log("i-cache isolated access ~%08x", address);
      return io_target::icache;
    } else {
      log("d-cache isolated access ~%08x", address);
      return io_target::dcache;
    }
  }

  if ((address & 0x7ffffc00) == 0x1f800000 && get_segment(address) < segment::kseg1) {
    log("d-cache memory access ~%08x", address);
    return io_target::dcache;
  }

  return io_target::memory;
}

uint32_t core::read_data(address_width width, uint32_t address) {
  address = map_address(address);

  switch (get_target(address)) {
    case io_target::icache: return 0;
    case io_target::dcache: return dcache.io_read(width, address);
    case io_target::memory: return memory.io_read(width, address);
  }

  return 0;
}

void core::write_data(address_width width, uint32_t address, uint32_t data) {
  address = map_address(address);

  switch (get_target(address)) {
    case io_target::icache: break;
    case io_target::dcache: dcache.io_write(width, address, data); break;
    case io_target::memory: memory.io_write(width, address, data); break;
  }
}

void core::update_irq(uint32_t stat, uint32_t mask) {
  log("update irq: stat=0x%08x, mask=0x%08x", stat, mask);

  auto cop0 = (cop0::sys *) get_cop(0);

  istat = stat;
  imask = mask;

  if (istat & imask) {
    cop0->put_cause_ip(1);
  } else {
    cop0->put_cause_ip(0);
  }
}

uint32_t core::get_imask() const {
  return imask;
}

void core::set_imask(uint32_t value) {
  update_irq(get_istat(), value);
}

uint32_t core::get_istat() const {
  return istat;
}

void core::set_istat(uint32_t value) {
  update_irq(value, get_imask());
}

uint32_t core::io_read(address_width width, uint32_t address) {
  timing::add_cpu_time(4);

  if (width == address_width::word || width == address_width::half) {
    switch (address) {
      case 0x1f801070:
        return get_istat();

      case 0x1f801074:
        return get_imask();
    }
  }

  return addressable::io_read(width, address);
}

void core::io_write(address_width width, uint32_t address, uint32_t data) {
  timing::add_cpu_time(4);

  if (width == address_width::word || width == address_width::half) {
    switch (address) {
      case 0x1f801070:
        return set_istat(data & istat);

      case 0x1f801074:
        return set_imask(data & 0x7ff);
    }
  }

  return addressable::io_write(width, address, data);
}

// --========--
//   Decoding
// --========--

static inline uint32_t overflow(uint32_t x, uint32_t y, uint32_t z) {
  return (~(x ^ y) & (x ^ z) & 0x80000000);
}

uint32_t core::get_code() const {
  return code;
}

uint32_t core::get_pc() const {
  return regs.pc;
}

uint32_t core::get_rt() const {
  return get_register(decode_rt());
}

uint32_t core::get_rt_forwarded() const {
  return get_register_forwarded(decode_rt());
}

uint32_t core::get_rs() const {
  return get_register(decode_rs());
}

uint32_t core::get_register(uint32_t index) const {
  if (is_load_delay_slot && load_index == index) {
    return load_value;
  } else {
    return rf.get(index);
  }
}

uint32_t core::get_register_forwarded(uint32_t index) const {
  return rf.get(index);
}

void core::set_pc(uint32_t value) {
  regs.this_pc = value;
  regs.pc = value;
  regs.next_pc = value + sizeof(uint32_t);
}

void core::set_rd(uint32_t value) {
  rf.put(decode_rd(), value);
}

void core::set_rt(uint32_t value) {
  rf.put(decode_rt(), value);
}

void core::set_rt_load(uint32_t value) {
  uint32_t t = decode_rt();

  if (is_load_delay_slot && load_index == t) {
    rf.put(t, load_value);
  }

  is_load = true;
  load_index = t;
  load_value = rf.get(t);

  rf.put(t, value);
}

void core::set_register(uint32_t index, uint32_t value) {
  rf.put(index, value);
}

void core::branch(uint32_t target, bool condition) {
  is_branch = true;
  is_branch_taken = condition;
  branch_target = target;

  if (condition) {
    regs.next_pc = target;
  }
}

uint32_t core::branch_abs() {
  return (regs.pc & 0xf0000000) | ((get_code() << 2) & 0x0ffffffc);
}

uint32_t core::branch_rel() {
  return regs.pc + (decode_iconst() << 2);
}

// --============--
//   Instructions
// --============--

void core::op_add() {
  uint32_t x = get_rs();
  uint32_t y = get_rt();
  uint32_t z = x + y;

  if (overflow(x, y, z)) {
    return enter_exception(cop0::exception::overflow, 0);
  }

  set_rd(z);
}

void core::op_addi() {
  uint32_t x = get_rs();
  uint32_t y = decode_iconst();
  uint32_t z = x + y;

  if (overflow(x, y, z)) {
    return enter_exception(cop0::exception::overflow, 0);
  }

  set_rt(z);
}

void core::op_addiu() {
  set_rt(get_rs() + decode_iconst());
}

void core::op_addu() {
  set_rd(get_rs() + get_rt());
}

void core::op_and() {
  set_rd(get_rs() & get_rt());
}

void core::op_andi() {
  set_rt(get_rs() & decode_uconst());
}

void core::op_beq() {
  branch(branch_rel(), get_rs() == get_rt());
}

void core::op_bgtz() {
  branch(branch_rel(), int32_t(get_rs()) > 0);
}

void core::op_blez() {
  branch(branch_rel(), int32_t(get_rs()) <= 0);
}

void core::op_bne() {
  branch(branch_rel(), get_rs() != get_rt());
}

void core::op_break() {
  enter_exception(cop0::exception::breakpoint, 0);
}

void core::op_bxx() {
  // bgez rs,$nnnn
  // bgezal rs,$nnnn
  // bltz rs,$nnnn
  // bltzal rs,$nnnn
  bool condition = (get_code() & (1 << 16))
    ? int32_t(get_rs()) >= 0
    : int32_t(get_rs()) <  0;

  if ((get_code() & 0x1e0000) == 0x100000) {
    rf.put(31, regs.next_pc);
  }

  branch(branch_rel(), condition);
}

void core::op_cop(int n) {
  if (get_cop_usable(n) == false) {
    return enter_exception(cop0::exception::cop_unusable, n);
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

void core::op_cop0() {
  op_cop(0);
}

void core::op_cop1() {
  op_cop(1);
}

void core::op_cop2() {
  op_cop(2);
}

void core::op_cop3() {
  op_cop(3);
}

void core::op_div() {
  int32_t dividend = int32_t(get_rs());
  int32_t divisor = int32_t(get_rt());

  if (dividend == int32_t(0x80000000) && divisor == int32_t(0xffffffff)) {
    regs.lo = 0x80000000;
    regs.hi = 0;
  } else if (dividend >= 0 && divisor == 0) {
    regs.lo = uint32_t(0xffffffff);
    regs.hi = uint32_t(dividend);
  } else if (dividend <= 0 && divisor == 0) {
    regs.lo = uint32_t(0x00000001);
    regs.hi = uint32_t(dividend);
  } else {
    regs.lo = uint32_t(dividend / divisor);
    regs.hi = uint32_t(dividend % divisor);
  }
}

void core::op_divu() {
  uint32_t dividend = get_rs();
  uint32_t divisor = get_rt();

  if (divisor) {
    regs.lo = dividend / divisor;
    regs.hi = dividend % divisor;
  } else {
    regs.lo = 0xffffffff;
    regs.hi = dividend;
  }
}

void core::op_j() {
  branch(branch_abs(), true);
}

void core::op_jal() {
  rf.put(31, regs.next_pc);
  branch(branch_abs(), true);
}

void core::op_jalr() {
  uint32_t ra = regs.next_pc;
  branch(get_rs(), true);

  set_rd(ra);
}

void core::op_jr() {
  branch(get_rs(), true);
}

void core::op_lb() {
  uint32_t address = get_rs() + decode_iconst();
  uint32_t data = read_data(address_width::byte, address);
  data = int_t<8>::trunc(data);

  set_rt_load(data);
}

void core::op_lbu() {
  uint32_t address = get_rs() + decode_iconst();
  uint32_t data = read_data(address_width::byte, address);
  data = uint_t<8>::trunc(data);

  set_rt_load(data);
}

void core::op_lh() {
  uint32_t address = get_rs() + decode_iconst();
  if (address & 1) {
    return enter_exception(cop0::exception::address_error_load, 0);
  }

  uint32_t data = read_data(address_width::half, address);
  data = int_t<16>::trunc(data);

  set_rt_load(data);
}

void core::op_lhu() {
  uint32_t address = get_rs() + decode_iconst();
  if (address & 1) {
    return enter_exception(cop0::exception::address_error_load, 0);
  }

  uint32_t data = read_data(address_width::half, address);
  data = uint_t<16>::trunc(data);

  set_rt_load(data);
}

void core::op_lui() {
  set_rt(decode_uconst() << 16);
}

void core::op_lw() {
  uint32_t address = get_rs() + decode_iconst();
  if (address & 3) {
    return enter_exception(cop0::exception::address_error_load, 0);
  }

  uint32_t data = read_data(address_width::word, address);

  set_rt_load(data);
}

void core::op_lwc(int n) {
  if (get_cop_usable(n) == false) {
    return enter_exception(cop0::exception::cop_unusable, n);
  }

  uint32_t address = get_rs() + decode_iconst();
  if (address & 3) {
    return enter_exception(cop0::exception::address_error_load, 0);
  }

  get_cop(n)->write_gpr(decode_rt(), read_data(address_width::word, address));
}

void core::op_lwc0() {
  op_lwc(0);
}

void core::op_lwc1() {
  op_lwc(1);
}

void core::op_lwc2() {
  op_lwc(2);
}

void core::op_lwc3() {
  op_lwc(3);
}

void core::op_lwl() {
  uint32_t address = get_rs() + decode_iconst();
  uint32_t data = read_data(address_width::word, address & ~3);

  switch (address & 3) {
    case 0: data = (data << 24) | (get_rt_forwarded() & 0x00ffffff); break;
    case 1: data = (data << 16) | (get_rt_forwarded() & 0x0000ffff); break;
    case 2: data = (data <<  8) | (get_rt_forwarded() & 0x000000ff); break;
    case 3: data = (data <<  0) | (get_rt_forwarded() & 0x00000000); break;
  }

  set_rt_load(data);
}

void core::op_lwr() {
  uint32_t address = get_rs() + decode_iconst();
  uint32_t data = read_data(address_width::word, address & ~3);

  switch (address & 3) {
    case 0: data = (data >>  0) | (get_rt_forwarded() & 0x00000000); break;
    case 1: data = (data >>  8) | (get_rt_forwarded() & 0xff000000); break;
    case 2: data = (data >> 16) | (get_rt_forwarded() & 0xffff0000); break;
    case 3: data = (data >> 24) | (get_rt_forwarded() & 0xffffff00); break;
  }

  set_rt_load(data);
}

void core::op_mfhi() {
  set_rd(regs.hi);
}

void core::op_mflo() {
  set_rd(regs.lo);
}

void core::op_mthi() {
  regs.hi = get_rs();
}

void core::op_mtlo() {
  regs.lo = get_rs();
}

void core::op_mult() {
  int32_t rs = int32_t(get_rs());
  int32_t rt = int32_t(get_rt());

  int64_t result = int64_t(rs) * int64_t(rt);
  regs.lo = uint32_t(result >> 0);
  regs.hi = uint32_t(result >> 32);
}

void core::op_multu() {
  uint32_t s = get_rs();
  uint32_t t = get_rt();

  uint64_t result = uint64_t(s) * uint64_t(t);
  regs.lo = uint32_t(result >> 0);
  regs.hi = uint32_t(result >> 32);
}

void core::op_nor() {
  set_rd(~(get_rs() | get_rt()));
}

void core::op_or() {
  set_rd(get_rs() | get_rt());
}

void core::op_ori() {
  set_rt(get_rs() | decode_uconst());
}

void core::op_sb() {
  uint32_t address = get_rs() + decode_iconst();
  uint32_t data = get_rt();

  write_data(address_width::byte, address, data);
}

void core::op_sh() {
  uint32_t address = get_rs() + decode_iconst();
  if (address & 1) {
    return enter_exception(cop0::exception::address_error_store, 0);
  }

  write_data(address_width::half, address, get_rt());
}

void core::op_sll() {
  set_rd(get_rt() << decode_sa());
}

void core::op_sllv() {
  set_rd(get_rt() << get_rs());
}

void core::op_slt() {
  set_rd(int32_t(get_rs()) < int32_t(get_rt()) ? 1 : 0);
}

void core::op_slti() {
  set_rt(int32_t(get_rs()) < int32_t(decode_iconst()) ? 1 : 0);
}

void core::op_sltiu() {
  set_rt(get_rs() < decode_iconst() ? 1 : 0);
}

void core::op_sltu() {
  set_rd(get_rs() < get_rt() ? 1 : 0);
}

void core::op_sra() {
  set_rd(int32_t(get_rt()) >> decode_sa());
}

void core::op_srav() {
  set_rd(int32_t(get_rt()) >> get_rs());
}

void core::op_srl() {
  set_rd(get_rt() >> decode_sa());
}

void core::op_srlv() {
  set_rd(get_rt() >> get_rs());
}

void core::op_sub() {
  uint32_t x = get_rs();
  uint32_t y = get_rt();
  uint32_t z = x - y;

  if (overflow(x, ~y, z)) {
    return enter_exception(cop0::exception::overflow, 0);
  }

  set_rd(z);
}

void core::op_subu() {
  set_rd(get_rs() - get_rt());
}

void core::op_sw() {
  uint32_t address = get_rs() + decode_iconst();
  if (address & 3) {
    return enter_exception(cop0::exception::address_error_store, 0);
  }

  uint32_t data = get_rt();

  write_data(address_width::word, address, data);
}

void core::op_swc(int n) {
  if (get_cop_usable(n) == false) {
    return enter_exception(cop0::exception::cop_unusable, n);
  }

  uint32_t address = get_rs() + decode_iconst();
  if (address & 3) {
    return enter_exception(cop0::exception::address_error_store, 0);
  }

  write_data(address_width::word, address, get_cop(n)->read_gpr(decode_rt()));
}

void core::op_swc0() {
  op_swc(0);
}

void core::op_swc1() {
  op_swc(1);
}

void core::op_swc2() {
  op_swc(2);
}

void core::op_swc3() {
  op_swc(3);
}

void core::op_swl() {
  uint32_t address = get_rs() + decode_iconst();
  uint32_t data = read_data(address_width::word, address & ~3);

  switch (address & 3) {
    case 0: data = (data & 0xffffff00) | (get_rt() >> 24); break;
    case 1: data = (data & 0xffff0000) | (get_rt() >> 16); break;
    case 2: data = (data & 0xff000000) | (get_rt() >>  8); break;
    case 3: data = (data & 0x00000000) | (get_rt() >>  0); break;
  }

  write_data(address_width::word, address & ~3, data);
}

void core::op_swr() {
  uint32_t address = get_rs() + decode_iconst();
  uint32_t data = read_data(address_width::word, address & ~3);

  switch (address & 3) {
    case 0: data = (data & 0x00000000) | (get_rt() <<  0); break;
    case 1: data = (data & 0x000000ff) | (get_rt() <<  8); break;
    case 2: data = (data & 0x0000ffff) | (get_rt() << 16); break;
    case 3: data = (data & 0x00ffffff) | (get_rt() << 24); break;
  }

  write_data(address_width::word, address & ~3, data);
}

void core::op_syscall() {
  enter_exception(cop0::exception::syscall, 0);
}

void core::op_xor() {
  set_rd(get_rs() ^ get_rt());
}

void core::op_xori() {
  set_rt(get_rs() ^ decode_uconst());
}

void core::op_und() {
  enter_exception(cop0::exception::reserved_instruction, 0);
}
