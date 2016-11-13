#include <stdexcept>
#include "cpu_core.hpp"
#include "../bus.hpp"

cpu::state_t cpu::state;

void cpu::initialize() {
  state.regs.gp[0] = 0;
  state.regs.pc = 0xbfc00000;
  state.regs.next_pc = state.regs.pc + 4;
}

static void (*op_table[64])() = {
  cpu::op_special, cpu::op_reg_imm, cpu::op_j,    cpu::op_jal,   cpu::op_beq,  cpu::op_bne, cpu::op_blez, cpu::op_bgtz,
  cpu::op_addi,    cpu::op_addiu,   cpu::op_slti, cpu::op_sltiu, cpu::op_andi, cpu::op_ori, cpu::op_xori, cpu::op_lui,
  cpu::op_cop0,    cpu::op_cop1,    cpu::op_cop2, cpu::op_cop3,  cpu::op_und,  cpu::op_und, cpu::op_und,  cpu::op_und,
  cpu::op_und,     cpu::op_und,     cpu::op_und,  cpu::op_und,   cpu::op_und,  cpu::op_und, cpu::op_und,  cpu::op_und,
  cpu::op_lb,      cpu::op_lh,      cpu::op_lwl,  cpu::op_lw,    cpu::op_lbu,  cpu::op_lhu, cpu::op_lwr,  cpu::op_und,
  cpu::op_sb,      cpu::op_sh,      cpu::op_swl,  cpu::op_sw,    cpu::op_und,  cpu::op_und, cpu::op_swr,  cpu::op_und,
  cpu::op_lwc0,    cpu::op_lwc1,    cpu::op_lwc2, cpu::op_lwc3,  cpu::op_und,  cpu::op_und, cpu::op_und,  cpu::op_und,
  cpu::op_swc0,    cpu::op_swc1,    cpu::op_swc2, cpu::op_swc3,  cpu::op_und,  cpu::op_und, cpu::op_und,  cpu::op_und
};

void cpu::run(int count) {
  for (int i = 0; i < count; i++) {
    cpu::read_code();

    state.is_branch_delay_slot = state.is_branch;
    state.is_branch = false;

    state.is_load_delay_slot = state.is_load;
    state.is_load = false;

    op_table[(cpu::state.code >> 26) & 63]();
  }
}

void cpu::enter_exception(uint32_t code) {
  uint32_t status = state.cop0.regs[12];
  status = (status & ~0x3f) | ((status << 2) & 0x3f);

  uint32_t cause = state.cop0.regs[13];
  cause = (cause & ~0x7f) | ((code << 2) & 0x7f);

  auto epc = state.regs.this_pc;

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

  state.code = bus::read_word(map_address(state.regs.this_pc));
}

uint32_t cpu::read_data_byte(uint32_t address) {
  if (state.cop0.regs[12] & (1 << 16)) {
    return 0; // isc=1
  }

  // todo: read d-cache?

  return bus::read_byte(map_address(address));
}

uint32_t cpu::read_data_half(uint32_t address) {
  if (state.cop0.regs[12] & (1 << 16)) {
    return 0; // isc=1
  }

  // todo: read d-cache?

  return bus::read_half(map_address(address));
}

uint32_t cpu::read_data_word(uint32_t address) {
  if (state.cop0.regs[12] & (1 << 16)) {
    return 0; // isc=1
  }

  // todo: read d-cache?

  return bus::read_word(map_address(address));
}

void cpu::write_data_byte(uint32_t address, uint32_t data) {
  if (state.cop0.regs[12] & (1 << 16)) {
    return; // isc=1
  }

  // todo: write d-cache?

  return bus::write(BYTE, map_address(address), data);
}

void cpu::write_data_half(uint32_t address, uint32_t data) {
  if (state.cop0.regs[12] & (1 << 16)) {
    return; // isc=1
  }

  // todo: write d-cache?

  return bus::write(HALF, map_address(address), data);
}

void cpu::write_data_word(uint32_t address, uint32_t data) {
  if (state.cop0.regs[12] & (1 << 16)) {
    return; // isc=1
  }

  // todo: write d-cache?

  return bus::write(WORD, map_address(address), data);
}

uint32_t cpu::mmio_read(int, uint32_t address) {
  switch (address - 0x1f801070) {
    case 0: return state.i_stat;
    case 4: return state.i_mask;
  }
}

void cpu::mmio_write(int, uint32_t address, uint32_t data) {
  switch (address - 0x1f801070) {
    case 0: state.i_stat = state.i_stat & data; break;
    case 4: state.i_mask = data; break;
  }
}
