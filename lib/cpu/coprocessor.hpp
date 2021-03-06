#ifndef CPU_COPROCESSOR_HPP_
#define CPU_COPROCESSOR_HPP_

#include <cstdint>

namespace psx::cpu {

  class coprocessor {
  public:
    virtual void run(uint32_t code) = 0;

    virtual uint32_t read_ccr(uint32_t n) = 0;
    virtual void write_ccr(uint32_t n, uint32_t value) = 0;

    virtual uint32_t read_gpr(uint32_t n) = 0;
    virtual void write_gpr(uint32_t n, uint32_t value) = 0;
  };
}

#endif
