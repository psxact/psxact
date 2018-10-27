#ifndef __psxact_cpu_cop0__
#define __psxact_cpu_cop0__


#include <cstdint>
#include "cpu/cpu-cop.hpp"


namespace psx {

enum class cop0_exception_code_t {
  interrupt,
  tlb_modification,
  tlb_load,
  tlb_store,
  address_error_load,
  address_error_store,
  instruction_bus_error,
  data_bus_error,
  syscall,
  breakpoint,
  reserved_instruction,
  cop_unusable,
  overflow
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

}

#endif //__psxact_cpu_cop0__
