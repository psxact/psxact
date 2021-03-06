#ifndef UTIL_UINT_HPP_
#define UTIL_UINT_HPP_

#include <cstdint>
#include <cstdio>

namespace psx::util {

  template<int bits>
  struct uint_t {
    static constexpr int64_t mask = (1LL << bits) - 1;
    static constexpr int64_t msb  = 1LL << (bits - 1);
    static constexpr int64_t min  = 0LL;
    static constexpr int64_t max  = (1LL << bits) - 1;

    static int64_t trunc(int64_t value) {
      return value & mask;
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
}

#endif
