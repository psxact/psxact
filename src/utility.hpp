#ifndef __psxact_utility_hpp__
#define __psxact_utility_hpp__


#include <cstdio>
#include <cstdint>
#include "memory.hpp"


#define likely(n) __builtin_expect(!!(n), 1)
#define unlikely(n) __builtin_expect(!!(n), 0)


namespace utility {
  const bool log_cdrom = 0;
  const bool log_cpu   = 0;
  const bool log_dma   = 0;
  const bool log_gpu   = 0;
  const bool log_input = 0;
  const bool log_spu   = 0;
  const bool log_timer = 0;

  template<int bits>
  bool read_all_bytes(const char *filename, memory_t<bits> &memory) {
    if (FILE* file = fopen(filename, "rb+")) {
      fread(memory.b, sizeof(uint8_t), memory.size, file);
      fclose(file);
      return true;
    }
    else {
      return false;
    }
  }

  template<int bits>
  inline uint32_t sclip(uint32_t value) {
    const int mask = (1 << bits) - 1;
    const int sign = 1 << (bits - 1);

    return ((value & mask) ^ sign) - sign;
  }

  template<int bits>
  inline uint32_t uclip(uint32_t value) {
    const int mask = (1 << bits) - 1;

    return value & mask;
  }

  inline uint8_t dec_to_bcd(uint8_t value) {
    return uint8_t(((value / 10) * 16) + (value % 10));
  }

  inline uint8_t bcd_to_dec(uint8_t value) {
    return uint8_t(((value / 16) * 10) + (value % 16));
  }

  template<int bits>
  inline int clz(uint32_t value) {
    enum {
      mask = 1 << (bits - 1)
    };

    uint32_t count = 0;

    while (!(value & mask) && count < bits) {
      value = value << 1;
      count = count + 1;
    }

    return count;
  }
}


#endif // __psxact_utility_hpp__
