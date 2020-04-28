#ifndef CPU_COP0_SYS_HPP_
#define CPU_COP0_SYS_HPP_

#include <cstdint>
#include "cpu/cop.hpp"

namespace psx::cpu::cop0 {

enum status_flags_t {
  ISC = 1 << 16,
  SWC = 1 << 17
};

enum class exception_t {
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

class sys_t : public cop_t {
  uint32_t regs[16];

 public:
  void run(uint32_t n);

  uint32_t read_ccr(uint32_t n);

  void write_ccr(uint32_t n, uint32_t value);

  uint32_t read_gpr(uint32_t n);

  void write_gpr(uint32_t n, uint32_t value);

  uint32_t enter_exception(exception_t code, uint32_t pc, bool is_branch_delay_slot);

  void rfe();
};

}  // namespace psx::cpu::cop0

#endif  // CPU_COP0_SYS_HPP_
