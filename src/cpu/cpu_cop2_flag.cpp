#include <cstdint>
#include "cpu_cop2.hpp"
#include "../limits.hpp"

static inline void set_flag(cop2_state_t *state, int flag) {
  state->ccr.flag |= 1 << flag;
}

static inline int32_t get_lm(uint32_t code) {
  return (code & (1 << 10))
         ? ulimit<15>::min
         : slimit<15>::min;
}

int64_t cop2::flags::a(cop2_state_t *state, int n, int64_t value) {
  const int64_t max = slimit<43>::max;
  const int64_t min = slimit<43>::min;

  if (value > max) {
    set_flag(state, A1_MAX - n);
  }
  else if (value < min) {
    set_flag(state, A1_MIN - n);
  }

  return (value << 20) >> 20;
}

int32_t cop2::flags::b(cop2_state_t *state, int n, uint32_t code, int32_t value) {
  int32_t max = slimit<15>::max;
  int32_t min = get_lm(code);

  if (value < min) {
    set_flag(state, B1 - n);
    return min;
  }

  if (value > max) {
    set_flag(state, B1 - n);
    return max;
  }

  return value;
}

int32_t cop2::flags::b(cop2_state_t *state, int n, uint32_t code, int32_t value, int32_t shifted) {
  const int32_t max = slimit<15>::max;
  const int32_t min = slimit<15>::min;

  if (shifted < min || shifted > max) {
    set_flag(state, B1 - n);
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

int32_t cop2::flags::c(cop2_state_t *state, int n, int32_t value) {
  const int32_t max = ulimit<8>::max;
  const int32_t min = ulimit<8>::min;

  if (value < min) {
    set_flag(state, C1 - n);
    return min;
  }

  if (value > max) {
    set_flag(state, C1 - n);
    return max;
  }

  return value;
}

int32_t cop2::flags::d(cop2_state_t *state, int32_t value) {
  const int32_t max = ulimit<16>::max;
  const int32_t min = ulimit<16>::min;

  if (value < min) {
    set_flag(state, D);
    return min;
  }

  if (value > max) {
    set_flag(state, D);
    return max;
  }

  return value;
}

//int32_t cop2::flags::d_chained(cop2_state_t *state, int32_t value) {
//  const int32_t max = ulimit<16>::max;
//  const int32_t min = ulimit<16>::min;
//
//  if (get_flag(state, F_MIN)) {
//    set_flag(state, D);
//    return min;
//  }
//
//  if (get_flag(state, F_MAX)) {
//    set_flag(state, D);
//    return max;
//  }
//
//  return value;
//}

int32_t cop2::flags::e(cop2_state_t *state) {
  set_flag(state, E);
  return 0x1ffff;
}

int64_t cop2::flags::f(cop2_state_t *state, int64_t value) {
  const int32_t max = slimit<31>::max;
  const int32_t min = slimit<31>::min;

  if (value < min) {
    set_flag(state, F_MIN);
  }

  if (value > max) {
    set_flag(state, F_MAX);
  }

  return value;
}

int32_t cop2::flags::g(cop2_state_t *state, int n, int32_t value) {
  const int32_t max = slimit<10>::max;
  const int32_t min = slimit<10>::min;

  if (value < min) {
    set_flag(state, G1 - n);
    return min;
  }

  if (value > max) {
    set_flag(state, G1 - n);
    return max;
  }

  return value;
}

int32_t cop2::flags::h(cop2_state_t *state, int64_t value) {
  const int32_t max = ulimit<12>::max + 1; // TODO: why is this one different?
  const int32_t min = ulimit<12>::min;

  if (value < min) {
    set_flag(state, H);
    return min;
  }

  if (value > max) {
    set_flag(state, H);
    return max;
  }

  return int32_t(value);
}
