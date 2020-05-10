#ifndef UTIL_INT_HPP_
#define UTIL_INT_HPP_

#include <cstdint>
#include <cstdio>

namespace psx::util {

  template<int bits>
  struct int_t {
    static constexpr int64_t mask = (1LL << bits) - 1;
    static constexpr int64_t msb  = 1LL << (bits - 1);
    static constexpr int64_t min  = 0LL - msb;
    static constexpr int64_t max  = msb - 1LL;

    static int64_t trunc(int64_t value) {
      return ((value & mask) ^ msb) - msb;
    }

    static int64_t clamp(int64_t value) {
      if (value < min) return min;
      if (value > max) return max;
      return value;
    }

    static int clz(int64_t value) {
      int count = 0;

      while (!(value & msb) && count < bits) {
        value = value << 1;
        count = count + 1;
      }

      return count;
    }
  };
}  // namespace psx::util

#endif  // UTIL_INT_HPP_
