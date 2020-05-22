#ifndef CPU_REGISTER_FILE_HPP_
#define CPU_REGISTER_FILE_HPP_

#include <cstdint>

namespace psx::cpu {

  class register_file {
    uint32_t registers[32];

  public:
    uint32_t get(uint32_t reg) const;
    void put(uint32_t reg, uint32_t val);
  };
}

#endif
