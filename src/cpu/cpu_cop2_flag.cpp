#include "cpu_cop2.hpp"
#include "../limits.hpp"

using namespace psxact::cpu::cop2;

void core::set_flag(int32_t flag) {
  ccr.flag |= 1 << flag;
}

static inline int32_t get_lm(uint32_t code) {
  return (code & (1 << 10))
         ? 0
         : slimit<16>::min;
}

int64_t core::flag_a(int32_t n, int64_t value) {
  const int64_t max = slimit<44>::max;
  const int64_t min = slimit<44>::min;

  if (value > max) {
    set_flag(A1_MAX - n);
  }
  else if (value < min) {
    set_flag(A1_MIN - n);
  }

  return (value << 20) >> 20;
}

int32_t core::flag_b(int32_t n, uint32_t code, int32_t value) {
  int32_t max = slimit<16>::max;
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

int32_t core::flag_b(int32_t n, uint32_t code, int32_t value, int32_t shifted) {
  const int32_t max = slimit<16>::max;
  const int32_t min = slimit<16>::min;

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

int32_t core::flag_c(int32_t n, int32_t value) {
  const int32_t max = ulimit<8>::max;
  const int32_t min = ulimit<8>::min;

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

int32_t core::flag_d(int32_t value) {
  const int32_t max = ulimit<16>::max;
  const int32_t min = ulimit<16>::min;

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

int32_t core::flag_e() {
  set_flag(E);
  return 0x1ffff;
}

int64_t core::flag_f(int64_t value) {
  const int32_t max = slimit<32>::max;
  const int32_t min = slimit<32>::min;

  if (value < min) {
    set_flag(F_MIN);
  }

  if (value > max) {
    set_flag(F_MAX);
  }

  return value;
}

int32_t core::flag_g(int32_t n, int32_t value) {
  const int32_t max = slimit<11>::max;
  const int32_t min = slimit<11>::min;

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

int32_t core::flag_h(int64_t value) {
  const int32_t max = ulimit<12>::max + 1; // TODO: why is this one different?
  const int32_t min = ulimit<12>::min;

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
