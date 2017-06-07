#ifndef __PSXACT_CPU_CORE_HPP__
#define __PSXACT_CPU_CORE_HPP__

#include <cstdint>
#include "../state.hpp"

namespace cpu {
  typedef void (*opcode)(cpu_state_t *);

  extern opcode op_table[64];
  extern opcode op_table_special[64];

  void initialize(cpu_state_t *state);

  void disassemble(cpu_state_t *state);

  void tick(cpu_state_t *state);

  void read_code(cpu_state_t *state);

  uint32_t read_data(cpu_state_t *state, int width, uint32_t address);

  void write_data(cpu_state_t *state, int width, uint32_t address, uint32_t data);

  void set_imask(cpu_state_t *state, uint32_t value);

  void set_istat(cpu_state_t *state, uint32_t value);

  uint32_t io_read(cpu_state_t *state, int width, uint32_t address);

  void io_write(cpu_state_t *state, int width, uint32_t address, uint32_t data);

  // --============--
  //   Instructions
  // --============--

  void op_add(cpu_state_t *state);
  void op_addi(cpu_state_t *state);
  void op_addiu(cpu_state_t *state);
  void op_addu(cpu_state_t *state);
  void op_and(cpu_state_t *state);
  void op_andi(cpu_state_t *state);
  void op_beq(cpu_state_t *state);
  void op_bgtz(cpu_state_t *state);
  void op_blez(cpu_state_t *state);
  void op_bne(cpu_state_t *state);
  void op_break(cpu_state_t *state);
  void op_bxx(cpu_state_t *state);
  void op_cop0(cpu_state_t *state);
  void op_cop1(cpu_state_t *state);
  void op_cop2(cpu_state_t *state);
  void op_cop3(cpu_state_t *state);
  void op_div(cpu_state_t *state);
  void op_divu(cpu_state_t *state);
  void op_j(cpu_state_t *state);
  void op_jal(cpu_state_t *state);
  void op_jalr(cpu_state_t *state);
  void op_jr(cpu_state_t *state);
  void op_lb(cpu_state_t *state);
  void op_lbu(cpu_state_t *state);
  void op_lh(cpu_state_t *state);
  void op_lhu(cpu_state_t *state);
  void op_lui(cpu_state_t *state);
  void op_lw(cpu_state_t *state);
  void op_lwc0(cpu_state_t *state);
  void op_lwc1(cpu_state_t *state);
  void op_lwc2(cpu_state_t *state);
  void op_lwc3(cpu_state_t *state);
  void op_lwl(cpu_state_t *state);
  void op_lwr(cpu_state_t *state);
  void op_mfhi(cpu_state_t *state);
  void op_mflo(cpu_state_t *state);
  void op_mthi(cpu_state_t *state);
  void op_mtlo(cpu_state_t *state);
  void op_mult(cpu_state_t *state);
  void op_multu(cpu_state_t *state);
  void op_nor(cpu_state_t *state);
  void op_or(cpu_state_t *state);
  void op_ori(cpu_state_t *state);
  void op_sb(cpu_state_t *state);
  void op_sh(cpu_state_t *state);
  void op_sll(cpu_state_t *state);
  void op_sllv(cpu_state_t *state);
  void op_slt(cpu_state_t *state);
  void op_slti(cpu_state_t *state);
  void op_sltiu(cpu_state_t *state);
  void op_sltu(cpu_state_t *state);
  void op_sra(cpu_state_t *state);
  void op_srav(cpu_state_t *state);
  void op_srl(cpu_state_t *state);
  void op_srlv(cpu_state_t *state);
  void op_sub(cpu_state_t *state);
  void op_subu(cpu_state_t *state);
  void op_sw(cpu_state_t *state);
  void op_swc0(cpu_state_t *state);
  void op_swc1(cpu_state_t *state);
  void op_swc2(cpu_state_t *state);
  void op_swc3(cpu_state_t *state);
  void op_swl(cpu_state_t *state);
  void op_swr(cpu_state_t *state);
  void op_syscall(cpu_state_t *state);
  void op_xor(cpu_state_t *state);
  void op_xori(cpu_state_t *state);

  // undefined instruction
  void op_und(cpu_state_t *state);

  namespace decoder {
    uint32_t iconst(cpu_state_t *state);
    uint32_t uconst(cpu_state_t *state);
    uint32_t sa(cpu_state_t *state);
    uint32_t rd(cpu_state_t *state);
    uint32_t rt(cpu_state_t *state);
    uint32_t rs(cpu_state_t *state);
  }
}

#endif // __PSXACT_CPU_CORE_HPP__
