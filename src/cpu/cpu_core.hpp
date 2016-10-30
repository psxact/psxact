#ifndef PSXACT_CPU_CORE_HPP
#define PSXACT_CPU_CORE_HPP

#include <cstdint>

namespace cpu {
  struct state_t {
    struct {
      uint32_t registers[16];
    } cop0;

    struct {
      union {
        uint32_t u[32];
//      int32_t i[32];
      };

      uint32_t lo;
      uint32_t hi;
      uint32_t pc;
      uint32_t next_pc;
    } registers;

    uint32_t code;

    bool is_branch;
    bool is_branch_delay_slot;
  };

  void initialize();

  void main();

  void enter_exception(uint32_t code, uint32_t epc);

  void leave_exception();

  uint32_t read_code();

  uint32_t read_data(int, uint32_t);

  void write_data(int, uint32_t, uint32_t);
}

#endif //PSXACT_CPU_CORE_HPP
