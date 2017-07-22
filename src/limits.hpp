#ifndef PSXACT_LIMITS_HPP
#define PSXACT_LIMITS_HPP

#include <cstdint>

template<typename T>
struct define {
  typedef T type;
};

template<typename T, typename F, bool n>
struct choice {};

template<typename T, typename F>
struct choice<T, F, 1> : define<T> {};

template<typename T, typename F>
struct choice<T, F, 0> : define<F> {};

template<int32_t bits>
struct int_type : choice<int32_t, int64_t, (bits < 31)> {};

template<int32_t bits>
struct slimit {
  typedef typename int_type<bits>::type type;

  static const type min = 0LL - (1LL << bits);
  static const type max = (1LL << bits) - 1LL;
};

template<int32_t bits>
struct ulimit {
  typedef typename int_type<bits>::type type;

  static const type min = 0LL;
  static const type max = (1LL << bits) - 1LL;
};

namespace limits {
  template<uint32_t min, uint32_t max>
  bool between(uint32_t value) {
    return (value & ~(min ^ max)) == min;
  }

  template<int32_t bits>
  int32_t sclamp(int32_t value) {
    const int min = slimit<bits>::min;
    const int max = slimit<bits>::max;

    if (value < min) return min;
    if (value > max) return max;
    return value;
  }

  template<int32_t bits>
  int32_t uclamp(int32_t value) {
    const int min = ulimit<bits>::min;
    const int max = ulimit<bits>::max;

    if (value < min) return min;
    if (value > max) return max;
    return value;
  }
}

#endif //PSXACT_LIMITS_HPP
