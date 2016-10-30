#ifndef PSXACT_UTILITY_HPP
#define PSXACT_UTILITY_HPP

#include <stdio.h>
#include <stdint.h>

namespace utility {
  void read_all_bytes(const char *, uint8_t *, int, int);

  uint32_t read_byte(uint8_t *, uint32_t);

  uint32_t read_half(uint8_t *, uint32_t);

  uint32_t read_word(uint8_t *, uint32_t);

  void write_byte(uint8_t *, uint32_t, uint32_t);

  void write_half(uint8_t *, uint32_t, uint32_t);

  void write_word(uint8_t *, uint32_t, uint32_t);

  template<int bits>
  inline uint32_t sclip(uint32_t value) {
    enum { mask = (1 << bits) - 1 };
    enum { sign = 1 << (bits - 1) };

    return ((value & mask) ^ sign) - sign;
  }

  template<int bits>
  inline uint32_t uclip(uint32_t value) {
    enum { mask = (1 << bits) - 1 };
    enum { sign = 0 };

    return ((value & mask) ^ sign) - sign;
  }

  template<unsigned min, unsigned max>
  inline bool between(unsigned value) {
    return (value & ~(min ^ max)) == min;
  }

  template<unsigned value>
  constexpr unsigned kib() {
    return 1024 * value;
  }

  template<unsigned value>
  constexpr unsigned mib() {
    return 1024 * kib<value>();
  }
}

#endif //PSXACT_UTILITY_HPP
