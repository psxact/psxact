#ifndef __psxact_cpu_cop0_hpp__
#define __psxact_cpu_cop0_hpp__


#include <cstdint>


enum class cop0_exception_code {
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


struct cop0_core {
  uint32_t regs[16];

  void run(uint32_t n);

  uint32_t read_ccr(uint32_t n);

  void write_ccr(uint32_t n, uint32_t value);

  uint32_t read_gpr(uint32_t n);

  void write_gpr(uint32_t n, uint32_t value);

  uint32_t enter_exception(cop0_exception_code code, uint32_t pc, bool is_branch_delay_slot);

  void leave_exception();
};


#endif //__psxact_cpu_cop0_hpp__
