#ifndef __psxact_cpu_cop0__
#define __psxact_cpu_cop0__


#include <cstdint>
#include "cpu/cpu-cop.hpp"


enum class cop0_exception_code_t {

  interrupt             = 0x0,
  tlb_modification      = 0x1,
  tlb_load              = 0x2,
  tlb_store             = 0x3,
  address_error_load    = 0x4,
  address_error_store   = 0x5,
  instruction_bus_error = 0x6,
  data_bus_error        = 0x7,
  syscall               = 0x8,
  breakpoint            = 0x9,
  reserved_instruction  = 0xa,
  cop_unusable          = 0xb,
  overflow              = 0xc

};


class cpu_cop0_t : public cpu_cop_t {

  uint32_t regs[16];

public:

  void run(uint32_t n);

  uint32_t read_ccr(uint32_t n);

  void write_ccr(uint32_t n, uint32_t value);

  uint32_t read_gpr(uint32_t n);

  void write_gpr(uint32_t n, uint32_t value);

  uint32_t enter_exception(cop0_exception_code_t code, uint32_t pc, bool is_branch_delay_slot);

  void rfe();

};


#endif //__psxact_cpu_cop0__
