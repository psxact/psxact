#ifndef PSXACT_LIMITS_HPP
#define PSXACT_LIMITS_HPP

#include <cstdint>

template<typename T>
struct _return {
  typedef T type;
};

template<typename T, typename F, bool n>
struct choice {};

template<typename T, typename F>
struct choice<T, F, 1> : _return<T> {};

template<typename T, typename F>
struct choice<T, F, 0> : _return<F> {};

template<int bits>
struct int_type : choice<int32_t, int64_t, (bits < 31)> {};

template<int bits>
struct slimit {
  typedef typename int_type<bits>::type type;

  static const type min = 0LL - (1LL << bits);
  static const type max = (1LL << bits) - 1LL;
};

template<int bits>
struct ulimit {
  typedef typename int_type<bits>::type type;

  static const type min = 0LL;
  static const type max = (1LL << bits) - 1LL;
};

#endif //PSXACT_LIMITS_HPP
