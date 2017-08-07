#include "cpu_core.hpp"
#include "cpu_cop0.hpp"
#include "../utility.hpp"

namespace psxact {
  cpu_core::opcode cpu_core::op_table[64] = {
      nullptr,            &cpu_core::op_bxx,   &cpu_core::op_j,    &cpu_core::op_jal,
      &cpu_core::op_beq,  &cpu_core::op_bne,   &cpu_core::op_blez, &cpu_core::op_bgtz,
      &cpu_core::op_addi, &cpu_core::op_addiu, &cpu_core::op_slti, &cpu_core::op_sltiu,
      &cpu_core::op_andi, &cpu_core::op_ori,   &cpu_core::op_xori, &cpu_core::op_lui,
      &cpu_core::op_cop0, &cpu_core::op_cop1,  &cpu_core::op_cop2, &cpu_core::op_cop3,
      &cpu_core::op_und,  &cpu_core::op_und,   &cpu_core::op_und,  &cpu_core::op_und,
      &cpu_core::op_und,  &cpu_core::op_und,   &cpu_core::op_und,  &cpu_core::op_und,
      &cpu_core::op_und,  &cpu_core::op_und,   &cpu_core::op_und,  &cpu_core::op_und,
      &cpu_core::op_lb,   &cpu_core::op_lh,    &cpu_core::op_lwl,  &cpu_core::op_lw,
      &cpu_core::op_lbu,  &cpu_core::op_lhu,   &cpu_core::op_lwr,  &cpu_core::op_und,
      &cpu_core::op_sb,   &cpu_core::op_sh,    &cpu_core::op_swl,  &cpu_core::op_sw,
      &cpu_core::op_und,  &cpu_core::op_und,   &cpu_core::op_swr,  &cpu_core::op_und,
      &cpu_core::op_lwc0, &cpu_core::op_lwc1,  &cpu_core::op_lwc2, &cpu_core::op_lwc3,
      &cpu_core::op_und,  &cpu_core::op_und,   &cpu_core::op_und,  &cpu_core::op_und,
      &cpu_core::op_swc0, &cpu_core::op_swc1,  &cpu_core::op_swc2, &cpu_core::op_swc3,
      &cpu_core::op_und,  &cpu_core::op_und,   &cpu_core::op_und,  &cpu_core::op_und
  };

  cpu_core::opcode cpu_core::op_table_special[64] = {
      &cpu_core::op_sll,     &cpu_core::op_und,   &cpu_core::op_srl,  &cpu_core::op_sra,
      &cpu_core::op_sllv,    &cpu_core::op_und,   &cpu_core::op_srlv, &cpu_core::op_srav,
      &cpu_core::op_jr,      &cpu_core::op_jalr,  &cpu_core::op_und,  &cpu_core::op_und,
      &cpu_core::op_syscall, &cpu_core::op_break, &cpu_core::op_und,  &cpu_core::op_und,
      &cpu_core::op_mfhi,    &cpu_core::op_mthi,  &cpu_core::op_mflo, &cpu_core::op_mtlo,
      &cpu_core::op_und,     &cpu_core::op_und,   &cpu_core::op_und,  &cpu_core::op_und,
      &cpu_core::op_mult,    &cpu_core::op_multu, &cpu_core::op_div,  &cpu_core::op_divu,
      &cpu_core::op_und,     &cpu_core::op_und,   &cpu_core::op_und,  &cpu_core::op_und,
      &cpu_core::op_add,     &cpu_core::op_addu,  &cpu_core::op_sub,  &cpu_core::op_subu,
      &cpu_core::op_and,     &cpu_core::op_or,    &cpu_core::op_xor,  &cpu_core::op_nor,
      &cpu_core::op_und,     &cpu_core::op_und,   &cpu_core::op_slt,  &cpu_core::op_sltu,
      &cpu_core::op_und,     &cpu_core::op_und,   &cpu_core::op_und,  &cpu_core::op_und,
      &cpu_core::op_und,     &cpu_core::op_und,   &cpu_core::op_und,  &cpu_core::op_und,
      &cpu_core::op_und,     &cpu_core::op_und,   &cpu_core::op_und,  &cpu_core::op_und,
      &cpu_core::op_und,     &cpu_core::op_und,   &cpu_core::op_und,  &cpu_core::op_und,
      &cpu_core::op_und,     &cpu_core::op_und,   &cpu_core::op_und,  &cpu_core::op_und
  };

  cpu_core::cpu_core() {
    regs.gp[0] = 0;
    regs.pc = 0xbfc00000;
    regs.next_pc = regs.pc + 4;
  }

  void cpu_core::tick() {
    read_code();

    is_branch_delay_slot = is_branch;
    is_branch = false;

    is_load_delay_slot = is_load;
    is_load = false;

    bool iec = (cop0.regs[12] & 1) != 0;
    bool irq = (cop0.regs[12] & cop0.regs[13] & 0xff00) != 0;

    if (iec && irq) {
      enter_exception(0x0);
    } else {
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

  void cpu_core::log_bios_calls() {
    if (regs.this_pc == 0x00a0) {
      printf("bios::a(0x%02x)\n", regs.gp[9]);
    } else if (regs.this_pc == 0x00b0) {
      printf("bios::b(0x%02x)\n", regs.gp[9]);
    } else if (regs.this_pc == 0x00c0) {
      printf("bios::c(0x%02x)\n", regs.gp[9]);
    }
  }

  void cpu_core::enter_exception(uint32_t code) {
    uint32_t pc = cop0.enter_exception(code, regs.this_pc, is_branch_delay_slot);

    regs.pc = pc;
    regs.next_pc = pc + 4;
  }

  void cpu_core::read_code() {
    if (regs.pc & 3) {
      enter_exception(0x4);
    }

    regs.this_pc = regs.pc;
    regs.pc = regs.next_pc;
    regs.next_pc += 4;

    // todo: read i-cache

    code = system->read(BUS_WIDTH_WORD, map_address(regs.this_pc));
  }

  uint32_t cpu_core::read_data(bus_width_t width, uint32_t address) {
    if (cop0.regs[12] & (1 << 16)) {
      return 0; // isc=1
    }

    // todo: read d-cache?

    return system->read(width, map_address(address));
  }

  void cpu_core::write_data(bus_width_t width, uint32_t address, uint32_t data) {
    if (cop0.regs[12] & (1 << 16)) {
      return; // isc=1
    }

    // todo: write d-cache?

    return system->write(width, map_address(address), data);
  }

  void cpu_core::update_irq(uint32_t stat, uint32_t mask) {
    i_stat = stat;
    i_mask = mask;

    if (i_stat & i_mask) {
      cop0.regs[13] |= (1 << 10);
    } else {
      cop0.regs[13] &= ~(1 << 10);
    }
  }

  void cpu_core::set_imask(uint32_t value) {
    update_irq(i_stat, value);
  }

  void cpu_core::set_istat(uint32_t value) {
    update_irq(value, i_mask);
  }

  uint32_t cpu_core::io_read(bus_width_t width, uint32_t address) {
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

  void cpu_core::io_write(bus_width_t width, uint32_t address, uint32_t data) {
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
}
