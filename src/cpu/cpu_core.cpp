#include <stdexcept>
#include "cpu_core.hpp"
#include "../bus.hpp"

cpu::state_t cpu::state;

cpu::opcode cpu::op_table[64] = {
  nullptr,      cpu::op_bxx,   cpu::op_j,    cpu::op_jal,   cpu::op_beq,  cpu::op_bne, cpu::op_blez, cpu::op_bgtz,
  cpu::op_addi, cpu::op_addiu, cpu::op_slti, cpu::op_sltiu, cpu::op_andi, cpu::op_ori, cpu::op_xori, cpu::op_lui,
  cpu::op_cop0, cpu::op_cop1,  cpu::op_cop2, cpu::op_cop3,  cpu::op_und,  cpu::op_und, cpu::op_und,  cpu::op_und,
  cpu::op_und,  cpu::op_und,   cpu::op_und,  cpu::op_und,   cpu::op_und,  cpu::op_und, cpu::op_und,  cpu::op_und,
  cpu::op_lb,   cpu::op_lh,    cpu::op_lwl,  cpu::op_lw,    cpu::op_lbu,  cpu::op_lhu, cpu::op_lwr,  cpu::op_und,
  cpu::op_sb,   cpu::op_sh,    cpu::op_swl,  cpu::op_sw,    cpu::op_und,  cpu::op_und, cpu::op_swr,  cpu::op_und,
  cpu::op_lwc0, cpu::op_lwc1,  cpu::op_lwc2, cpu::op_lwc3,  cpu::op_und,  cpu::op_und, cpu::op_und,  cpu::op_und,
  cpu::op_swc0, cpu::op_swc1,  cpu::op_swc2, cpu::op_swc3,  cpu::op_und,  cpu::op_und, cpu::op_und,  cpu::op_und
};

cpu::opcode cpu::op_table_special[64] = {
  cpu::op_sll,  cpu::op_und,   cpu::op_srl,  cpu::op_sra,  cpu::op_sllv,    cpu::op_und,   cpu::op_srlv, cpu::op_srav,
  cpu::op_jr,   cpu::op_jalr,  cpu::op_und,  cpu::op_und,  cpu::op_syscall, cpu::op_break, cpu::op_und,  cpu::op_und,
  cpu::op_mfhi, cpu::op_mthi,  cpu::op_mflo, cpu::op_mtlo, cpu::op_und,     cpu::op_und,   cpu::op_und,  cpu::op_und,
  cpu::op_mult, cpu::op_multu, cpu::op_div,  cpu::op_divu, cpu::op_und,     cpu::op_und,   cpu::op_und,  cpu::op_und,
  cpu::op_add,  cpu::op_addu,  cpu::op_sub,  cpu::op_subu, cpu::op_and,     cpu::op_or,    cpu::op_xor,  cpu::op_nor,
  cpu::op_und,  cpu::op_und,   cpu::op_slt,  cpu::op_sltu, cpu::op_und,     cpu::op_und,   cpu::op_und,  cpu::op_und,
  cpu::op_und,  cpu::op_und,   cpu::op_und,  cpu::op_und,  cpu::op_und,     cpu::op_und,   cpu::op_und,  cpu::op_und,
  cpu::op_und,  cpu::op_und,   cpu::op_und,  cpu::op_und,  cpu::op_und,     cpu::op_und,   cpu::op_und,  cpu::op_und
};

void cpu::initialize() {
  state.regs.gp[0] = 0;
  state.regs.pc = 0xbfc00000;
  state.regs.next_pc = state.regs.pc + 4;
}

void cpu::tick() {
  cpu::read_code();

  state.is_branch_delay_slot = state.is_branch;
  state.is_branch = false;

  state.is_load_delay_slot = state.is_load;
  state.is_load = false;

  if (state.i_stat & state.i_mask) {
    state.cop0.regs[13] |= (1 << 10);
  }
  else {
    state.cop0.regs[13] &= ~(1 << 10);
  }

  auto iec = (state.cop0.regs[12] & 1) != 0;
  auto irq = (state.cop0.regs[12] & state.cop0.regs[13] & 0xff00) != 0;

  if (iec && irq) {
    enter_exception(0x0);
  }
  else {
    auto code = (cpu::state.code >> 26) & 63;
    if (code)
      op_table[code]();
    else
      op_table_special[(cpu::state.code >> 0) & 63]();
  }
}

void cpu::enter_exception(uint32_t code) {
  uint32_t status = state.cop0.regs[12];
  status = (status & ~0x3f) | ((status << 2) & 0x3f);

  uint32_t cause = state.cop0.regs[13];
  cause = (cause & ~0x7f) | ((code << 2) & 0x7f);

  uint32_t epc;

  if (state.is_branch_delay_slot) {
    epc = state.regs.this_pc - 4;
    cause |= 0x80000000;
  }
  else {
    epc = state.regs.this_pc;
    cause &= ~0x80000000;
  }

  state.cop0.regs[12] = status;
  state.cop0.regs[13] = cause;
  state.cop0.regs[14] = epc;

  state.regs.pc = (status & (1 << 22))
    ? 0xbfc00180
    : 0x80000080
    ;

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
  0xffffffff, // kseg2 ($c000_0000 - $ffff_ffff)
  0xffffffff  //
};

static inline uint32_t map_address(uint32_t address) {
  return address & segments[address >> 29];
}

void cpu::read_code() {
  if (state.regs.pc & 3) {
    enter_exception(0x4);
  }

  state.regs.this_pc = state.regs.pc;
  state.regs.pc = state.regs.next_pc;
  state.regs.next_pc += 4;

  // todo: read i-cache

  state.code = bus::read(bus::BUS_WIDTH_WORD, map_address(state.regs.this_pc));
}

uint32_t cpu::read_data(int width, uint32_t address) {
  if (state.cop0.regs[12] & (1 << 16)) {
    return 0; // isc=1
  }

  // todo: read d-cache?

  return bus::read(width, map_address(address));
}

void cpu::write_data(int width, uint32_t address, uint32_t data) {
  if (state.cop0.regs[12] & (1 << 16)) {
    return; // isc=1
  }

  // todo: write d-cache?

  return bus::write(width, map_address(address), data);
}

uint32_t cpu::bus_read(int width, uint32_t address) {
  printf("cpu::bus_read(%d, 0x%08x)\n", width, address);

  switch (address) {
  case 0x1f801070:
    return state.i_stat;

  case 0x1f801074:
    return state.i_mask;

  default:
    return 0;
  }
}

void cpu::bus_write(int width, uint32_t address, uint32_t data) {
  printf("cpu::bus_write(%d, 0x%08x, 0x%08x)\n", width, address, data);

  switch (address) {
  case 0x1f801070:
    state.i_stat = state.i_stat & data;
    break;

  case 0x1f801074:
    state.i_mask = data & 0x7ff;
    break;
  }
}
