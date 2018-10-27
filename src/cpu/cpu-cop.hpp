#ifndef __psxact_cpu_cop__
#define __psxact_cpu_cop__


#include <cstdint>


namespace psx {

class cpu_cop_t {

public:

  virtual void run(uint32_t code) = 0;

  virtual uint32_t read_ccr(uint32_t n) = 0;

  virtual void write_ccr(uint32_t n, uint32_t value) = 0;

  virtual uint32_t read_gpr(uint32_t n) = 0;

  virtual void write_gpr(uint32_t n, uint32_t value) = 0;

};

}

#endif // __psxact_cpu_cop__
