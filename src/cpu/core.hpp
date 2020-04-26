#ifndef CPU_CORE_HPP_
#define CPU_CORE_HPP_

#include <cstdio>
#include "bios/decoder.hpp"
#include "cpu/cop.hpp"
#include "cpu/cop0/sys.hpp"
#include "cpu/cop2/gte.hpp"
#include "addressable.hpp"
#include "memory.hpp"

namespace psx::cpu {

enum class io_target_t {
  MEMORY,
  ICACHE,
  DCACHE
};

enum segment_t {
  KUSEG = 0,
  KSEG0 = 4,
  KSEG1 = 5,
  KSEG2 = 6
};

class core_t final : public addressable_t {
  bios::decoder_t bios_call;

  addressable_t *memory;

  cop_t *cop[4];

  memory_t< kib(1) > dcache;

  struct {
    uint32_t gp[32];
    uint32_t lo;
    uint32_t hi;
    uint32_t pc;
    uint32_t this_pc;
    uint32_t next_pc;
  } regs;

  uint32_t code;

  bool is_branch;
  bool is_branch_delay_slot;

  bool is_load;
  bool is_load_delay_slot;
  uint32_t load_index;
  uint32_t load_value;

  uint32_t istat;
  uint32_t imask;

  typedef void (core_t:: *opcode_t)();

  static opcode_t op_table[64];

  static opcode_t op_table_special[64];

 public:
  explicit core_t(addressable_t *memory, bool log_enabled);

  uint32_t get_code() const;

  cop_t *get_cop(int n);

  bool get_cop_usable(int n);

  void disassemble(FILE *file);

  void disassemble_special(FILE *file);

  void disassemble_reg_imm(FILE *file);

  int tick();

  void enter_exception(cop0::exception_t code);

  void log_bios_calls();

  void update_irq(uint32_t stat, uint32_t mask);

  void read_code();

  io_target_t get_target(uint32_t address);

  uint32_t read_data_byte(uint32_t address);

  uint32_t read_data_half(uint32_t address);

  uint32_t read_data_word(uint32_t address);

  void write_data_byte(uint32_t address, uint32_t data);

  void write_data_half(uint32_t address, uint32_t data);

  void write_data_word(uint32_t address, uint32_t data);

  uint32_t get_imask();

  void set_imask(uint32_t value);

  uint32_t get_istat();

  void set_istat(uint32_t value);

  uint16_t io_read_half(uint32_t address);

  uint32_t io_read_word(uint32_t address);

  void io_write_half(uint32_t address, uint16_t data);

  void io_write_word(uint32_t address, uint32_t data);

  // -============-
  //  Instructions
  // -============-

  void op_add();

  void op_addi();

  void op_addiu();

  void op_addu();

  void op_and();

  void op_andi();

  void op_beq();

  void op_bgtz();

  void op_blez();

  void op_bne();

  void op_break();

  void op_bxx();

  void op_cop(int n);

  void op_cop0();

  void op_cop1();

  void op_cop2();

  void op_cop3();

  void op_div();

  void op_divu();

  void op_j();

  void op_jal();

  void op_jalr();

  void op_jr();

  void op_lb();

  void op_lbu();

  void op_lh();

  void op_lhu();

  void op_lui();

  void op_lw();

  void op_lwc(int n);

  void op_lwc0();

  void op_lwc1();

  void op_lwc2();

  void op_lwc3();

  void op_lwl();

  void op_lwr();

  void op_mfhi();

  void op_mflo();

  void op_mthi();

  void op_mtlo();

  void op_mult();

  void op_multu();

  void op_nor();

  void op_or();

  void op_ori();

  void op_sb();

  void op_sh();

  void op_sll();

  void op_sllv();

  void op_slt();

  void op_slti();

  void op_sltiu();

  void op_sltu();

  void op_sra();

  void op_srav();

  void op_srl();

  void op_srlv();

  void op_sub();

  void op_subu();

  void op_sw();

  void op_swc(int n);

  void op_swc0();

  void op_swc1();

  void op_swc2();

  void op_swc3();

  void op_swl();

  void op_swr();

  void op_syscall();

  void op_xor();

  void op_xori();

  // undefined instruction

  void op_und();

  uint32_t decode_iconst();

  uint32_t decode_uconst();

  uint32_t decode_sa();

  uint32_t decode_rd();

  uint32_t decode_rt();

  uint32_t decode_rs();

  uint32_t get_pc();

  uint32_t get_rt();

  uint32_t get_rt_forwarded();

  uint32_t get_rs();

  uint32_t get_register(uint32_t index);

  uint32_t get_register_forwarded(uint32_t index);

  void set_pc(uint32_t value);

  void set_rd(uint32_t value);

  void set_rt(uint32_t value);

  void set_rt_load(uint32_t value);

  void set_register(uint32_t index, uint32_t value);
};

}  // namespace psx::cpu

#endif  // CPU_CORE_HPP_
