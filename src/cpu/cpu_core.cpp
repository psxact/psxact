#include "cpu_core.hpp"
#include "cpu_cop0.hpp"
#include "../utility.hpp"

bool log_cpu_instr = 0;

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

void cpu::init(cpu_state_t &state) {
  state.regs.gp[0] = 0;
  state.regs.pc = 0xbfc00000;
  state.regs.next_pc = state.regs.pc + 4;
}

void cpu::tick(cpu_state_t &state) {
  cpu::read_code(state);

  state.is_branch_delay_slot = state.is_branch;
  state.is_branch = false;

  state.is_load_delay_slot = state.is_load;
  state.is_load = false;

  bool iec = (state.cop0.regs[12] & 1) != 0;
  bool irq = (state.cop0.regs[12] & state.cop0.regs[13] & 0xff00) != 0;

  if (iec && irq) {
    cop0::enter_exception(state, 0x0);
  }
  else {
    if (log_cpu_instr) {
      cpu::disassemble(state, stdout);
    }

    uint32_t code = (state.code >> 26) & 63;
    if (code)
      op_table[code](state);
    else
      op_table_special[state.code & 63](state);
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

static void log_bios_calls(const cpu_state_t *state) {
  if (state->regs.this_pc == 0x00a0) {
    printf("bios::a(0x%02x)\n", state->regs.gp[9]);
  }
  else if (state->regs.this_pc == 0x00b0) {
    printf("bios::b(0x%02x)\n", state->regs.gp[9]);
  }
  else if (state->regs.this_pc == 0x00c0) {
    printf("bios::c(0x%02x)\n", state->regs.gp[9]);
  }
}

void cpu::read_code(cpu_state_t &state) {
  if (state.regs.pc & 3) {
    cop0::enter_exception(state, 0x4);
  }

  state.regs.this_pc = state.regs.pc;
  state.regs.pc = state.regs.next_pc;
  state.regs.next_pc += 4;

//  if (state.regs.this_pc == 0x00b0 && state.regs.gp[9] == 0x3d) {
//    printf("%c", state->regs.gp[4]);
//  }

//  if (utility::log_cpu) {
//    log_bios_calls(state);
//  }

  // todo: read i-cache

  state.code = bus::read(bus::BUS_WIDTH_WORD, map_address(state.regs.this_pc));
}

uint32_t cpu::read_data(cpu_state_t &state, bus::bus_width_t width, uint32_t address) {
  if (state.cop0.regs[12] & (1 << 16)) {
    return 0; // isc=1
  }

  // todo: read d-cache?

  return bus::read(width, map_address(address));
}

void cpu::write_data(cpu_state_t &state, bus::bus_width_t width, uint32_t address, uint32_t data) {
  if (state.cop0.regs[12] & (1 << 16)) {
    return; // isc=1
  }

  // todo: write d-cache?

  return bus::write(width, map_address(address), data);
}

static void update_irq(cpu_state_t &state, uint32_t stat, uint32_t mask) {
  state.i_stat = stat;
  state.i_mask = mask;

  if (state.i_stat & state.i_mask) {
    state.cop0.regs[13] |= (1 << 10);
  }
  else {
    state.cop0.regs[13] &= ~(1 << 10);
  }
}

void cpu::set_imask(cpu_state_t &state, uint32_t value) {
  update_irq(state, state.i_stat, value);
}

void cpu::set_istat(cpu_state_t &state, uint32_t value) {
  update_irq(state, value, state.i_mask);
}

uint32_t cpu::io_read(cpu_state_t &state, bus::bus_width_t width, uint32_t address) {
  if (utility::log_cpu) {
    printf("cpu::io_read(%d, 0x%08x)\n", width, address);
  }

  switch (address - 0x1f801070) {
  case 0:
    return state.i_stat;

  case 4:
    return state.i_mask;

  default:
    return 0;
  }
}

void cpu::io_write(cpu_state_t &state, bus::bus_width_t width, uint32_t address, uint32_t data) {
  if (utility::log_cpu) {
    printf("cpu::io_write(%d, 0x%08x, 0x%08x)\n", width, address, data);
  }

  switch (address - 0x1f801070) {
  case 0:
    set_istat(state, data & state.i_stat);
    break;

  case 4:
    set_imask(state, data & 0x7ff);
    break;
  }
}
