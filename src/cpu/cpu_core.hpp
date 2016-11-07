#ifndef PSXACT_CPU_CORE_HPP
#define PSXACT_CPU_CORE_HPP

#include <cstdint>

namespace cpu {
  struct state_t {
    struct {
      uint32_t regs[16];
    } cop0;

    struct {
      union {
        uint32_t gp[32];
      };

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

    uint32_t i_stat;
    uint32_t i_mask;
  };

  extern state_t state;

  void initialize();

  void disassemble();

  bool run(int count);

  void enter_exception(uint32_t code);

  void leave_exception();

  void read_code();

  uint32_t read_data(int size, uint32_t address);

  void write_data(int size, uint32_t address, uint32_t data);

  uint32_t mmio_read(int size, uint32_t address);

  void mmio_write(int size, uint32_t address, uint32_t data);
}

#endif //PSXACT_CPU_CORE_HPP
