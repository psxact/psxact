#ifndef LIMITS_HPP_
#define LIMITS_HPP_

#include <cstdint>

namespace psx {

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

template<int bits>
struct int_type : choice<int32_t, int64_t, (bits < 31)> {};

template<int bits, typename T = typename int_type<bits>::type>
class slimit {
  static const T val = (1LL << (bits - 1));
 public:
  static const T min = 0LL - val;
  static const T max = val - 1LL;

  static T clamp(T value) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
  }
};

template<int bits, typename T = typename int_type<bits>::type>
class ulimit {
 public:
  static const T min = 0LL;
  static const T max = (1LL << bits) - 1LL;

  static T clamp(T value) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
  }
};

namespace limits {
  template<uint32_t min, uint32_t max>
  bool between(uint32_t value) {
    return (value & ~(min ^ max)) == min;
  }
}

}  // namespace psx

#endif  // LIMITS_HPP_
