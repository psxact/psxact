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
    uint32_t bpc;
    uint32_t bda;
    uint32_t tar;
    uint32_t dcic;
    uint32_t bad_vaddr;
    uint32_t bdam;
    uint32_t bpcm;
    uint32_t sr;
    uint32_t cause;
    uint32_t epc;

  public:
    void run(uint32_t n);

    uint32_t read_ccr(uint32_t n);
    void write_ccr(uint32_t n, uint32_t value);

    uint32_t read_gpr(uint32_t n);
    void write_gpr(uint32_t n, uint32_t value);

    bool get_bev();

    void put_tar(uint32_t val);
    void put_cause_excode(exception_t excode);
    void put_cause_ip(bool val);
    void put_cause_ce(int32_t val);
    void put_cause_bt(bool val);
    void put_cause_bd(bool val);
    void put_epc(uint32_t val);

    void push_flags();
    void pop_flags();
  };
}

#endif
