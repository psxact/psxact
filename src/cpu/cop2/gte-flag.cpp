#include "cpu/cop2/gte.hpp"

#include "util/uint.hpp"
#include "util/int.hpp"

using namespace psx::cpu::cop2;
using namespace psx::util;

void gte_t::set_flag(int32_t flag) {
  ccr.flag |= 1 << flag;
}

static inline int32_t get_lm(uint32_t code) {
  return (code & (1 << 10))
    ? 0
    : int_t<16>::min;
}

int64_t gte_t::flag_a(int32_t n, int64_t value) {
  const int64_t max = int_t<44>::max;
  const int64_t min = int_t<44>::min;

  if (value > max) {
    set_flag(A1_MAX - n);
  }
  else if (value < min) {
    set_flag(A1_MIN - n);
  }

  return (value << 20) >> 20;
}

int32_t gte_t::flag_b(int32_t n, uint32_t code, int32_t value) {
  int32_t max = int_t<16>::max;
  int32_t min = get_lm(code);

  if (value < min) {
    set_flag(B1 - n);
    return min;
  }

  if (value > max) {
    set_flag(B1 - n);
    return max;
  }

  return value;
}

int32_t gte_t::flag_b(int32_t n, uint32_t code, int32_t value, int32_t shifted) {
  const int32_t max = int_t<16>::max;
  const int32_t min = int_t<16>::min;

  if (shifted < min || shifted > max) {
    set_flag(B1 - n);
  }

  int32_t tmp = get_lm(code);

  if (value < tmp) {
    return tmp;
  }

  if (value > max) {
    return max;
  }

  return value;
}

int32_t gte_t::flag_c(int32_t n, int32_t value) {
  const int32_t max = uint_t<8>::max;
  const int32_t min = uint_t<8>::min;

  if (value < min) {
    set_flag(C1 - n);
    return min;
  }

  if (value > max) {
    set_flag(C1 - n);
    return max;
  }

  return value;
}

int32_t gte_t::flag_d(int32_t value) {
  const int32_t max = uint_t<16>::max;
  const int32_t min = uint_t<16>::min;

  if (value < min) {
    set_flag(D);
    return min;
  }

  if (value > max) {
    set_flag(D);
    return max;
  }

  return value;
}

int32_t gte_t::flag_e() {
  set_flag(E);
  return 0x1ffff;
}

int64_t gte_t::flag_f(int64_t value) {
  const int32_t max = int_t<32>::max;
  const int32_t min = int_t<32>::min;

  if (value < min) {
    set_flag(F_MIN);
  }

  if (value > max) {
    set_flag(F_MAX);
  }

  return value;
}

int32_t gte_t::flag_g(int32_t n, int32_t value) {
  const int32_t max = int_t<11>::max;
  const int32_t min = int_t<11>::min;

  if (value < min) {
    set_flag(G1 - n);
    return min;
  }

  if (value > max) {
    set_flag(G1 - n);
    return max;
  }

  return value;
}

int32_t gte_t::flag_h(int64_t value) {
  const int32_t max = uint_t<12>::max + 1;  // TODO(Adam): why is this one different?
  const int32_t min = uint_t<12>::min;

  if (value < min) {
    set_flag(H);
    return min;
  }

  if (value > max) {
    set_flag(H);
    return max;
  }

  return int32_t(value);
}
