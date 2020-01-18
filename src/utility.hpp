#ifndef UTILITY_HPP_
#define UTILITY_HPP_

#include <cstdint>
#include <cstdio>

namespace psx::utility {

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
  const int mask = 1 << (bits - 1);

  uint32_t count = 0;

  while (!(value & mask) && count < bits) {
    value = value << 1;
    count = count + 1;
  }

  return count;
}

}  // namespace psx::utility

#endif  // UTILITY_HPP_
