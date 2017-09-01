#include "cpu_core.hpp"
#include "cpu_cop0.hpp"
#include "../utility.hpp"

using namespace psxact;
using namespace psxact::cpu;

core::opcode core::op_table[64] = {
  nullptr,        &core::op_bxx,   &core::op_j,    &core::op_jal,
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

core::core() {
  regs.gp[0] = 0;
  regs.pc = 0xbfc00000;
  regs.next_pc = regs.pc + 4;
}

void core::tick() {
  read_code();

  is_branch_delay_slot = is_branch;
  is_branch = false;

  is_load_delay_slot = is_load;
  is_load = false;

  bool iec = (cop0.regs[12] & 1) != 0;
  bool irq = (cop0.regs[12] & cop0.regs[13] & 0xff00) != 0;

  if (iec && irq) {
    enter_exception(cop0::exception_code_t::interrupt);
  }
  else {
    uint32_t code = (this->code >> 26) & 63;
    if (code)
      (*this.*op_table[code])();
    else
      (*this.*op_table_special[this->code & 63])();
  }
}

static uint32_t segments[8] = {
    0x7fffffff, // kuseg ($0000_0000 - $7fff_ffff)
    0x7fffffff, //
    0x7fffffff, //
    0x7fffffff, //
    0x1fffffff, // kseg0 ($8000_0000 - $9fff_ffff)
    0x1fffffff, // kseg1 ($a000_0000 - $bfff_ffff)
    0xffffffff, // kseg2 ($c000_0000 - $ffff_ffff)
    0xffffffff  //
};

static inline uint32_t map_address(uint32_t address) {
  return address & segments[address >> 29];
}

void core::log_bios_calls() {
  if (regs.this_pc == 0x00a0) {
    printf("bios::a(0x%02x)\n", regs.gp[9]);
  }
  else if (regs.this_pc == 0x00b0) {
    printf("bios::b(0x%02x)\n", regs.gp[9]);
  }
  else if (regs.this_pc == 0x00c0) {
    printf("bios::c(0x%02x)\n", regs.gp[9]);
  }
}

void core::enter_exception(cop0::exception_code_t code) {
  uint32_t pc = cop0.enter_exception(code, regs.this_pc, is_branch_delay_slot);

  regs.pc = pc;
  regs.next_pc = pc + 4;
}

void core::read_code() {
  if (regs.pc & 3) {
    enter_exception(cop0::exception_code_t::address_error_load);
  }

  regs.this_pc = regs.pc;
  regs.pc = regs.next_pc;
  regs.next_pc += 4;

  // todo: read i-cache

  code = system->read(BUS_WIDTH_WORD, map_address(regs.this_pc));
}

uint32_t core::read_data(bus_width_t width, uint32_t address) {
  if (cop0.regs[12] & (1 << 16)) {
    return 0; // isc=1
  }

  // todo: read d-cache?

  return system->read(width, map_address(address));
}

void core::write_data(bus_width_t width, uint32_t address, uint32_t data) {
  if (cop0.regs[12] & (1 << 16)) {
    return; // isc=1
  }

  // todo: write d-cache?

  return system->write(width, map_address(address), data);
}

void core::update_irq(uint32_t stat, uint32_t mask) {
  i_stat = stat;
  i_mask = mask;

  if (i_stat & i_mask) {
    cop0.regs[13] |= (1 << 10);
  }
  else {
    cop0.regs[13] &= ~(1 << 10);
  }
}

void core::set_imask(uint32_t value) {
  update_irq(i_stat, value);
}

void core::set_istat(uint32_t value) {
  update_irq(value, i_mask);
}

uint32_t core::io_read(bus_width_t width, uint32_t address) {
  if (utility::log_cpu) {
    printf("cpu_core::io_read(%d, 0x%08x)\n", width, address);
  }

  switch (address - 0x1f801070) {
  case 0:
    return i_stat;

  case 4:
    return i_mask;

  default:
    return 0;
  }
}

void core::io_write(bus_width_t width, uint32_t address, uint32_t data) {
  if (utility::log_cpu) {
    printf("cpu_core::io_write(%d, 0x%08x, 0x%08x)\n", width, address, data);
  }

  switch (address - 0x1f801070) {
  case 0:
    set_istat(data & i_stat);
    break;

  case 4:
    set_imask(data & 0x7ff);
    break;
  }
}
