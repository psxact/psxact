#ifndef __PSXACT_CPU_COP2_HPP__
#define __PSXACT_CPU_COP2_HPP__

#include "../state.hpp"

namespace cop2 {
  void run(cop2_state_t *state, uint32_t code);

  uint32_t read_ccr(cop2_state_t *state, uint32_t n);
  void write_ccr(cop2_state_t *state, uint32_t n, uint32_t value);

  uint32_t read_gpr(cop2_state_t *state, uint32_t n);
  void write_gpr(cop2_state_t *state, uint32_t n, uint32_t value);

  uint32_t divide(cop2_state_t *state);

  // -============-
  //  Instructions
  // -============-

  void op_avsz3(cop2_state_t *state, uint32_t code);
  void op_avsz4(cop2_state_t *state, uint32_t code);
  void op_cc(cop2_state_t *state, uint32_t code);
  void op_cdp(cop2_state_t *state, uint32_t code);
  void op_dcpl(cop2_state_t *state, uint32_t code);
  void op_dpcs(cop2_state_t *state, uint32_t code);
  void op_dpct(cop2_state_t *state, uint32_t code);
  void op_gpf(cop2_state_t *state, uint32_t code);
  void op_gpl(cop2_state_t *state, uint32_t code);
  void op_intpl(cop2_state_t *state, uint32_t code);
  void op_mvmva(cop2_state_t *state, uint32_t code);
  void op_nccs(cop2_state_t *state, uint32_t code);
  void op_ncct(cop2_state_t *state, uint32_t code);
  void op_ncds(cop2_state_t *state, uint32_t code);
  void op_ncdt(cop2_state_t *state, uint32_t code);
  void op_nclip(cop2_state_t *state, uint32_t code);
  void op_ncs(cop2_state_t *state, uint32_t code);
  void op_nct(cop2_state_t *state, uint32_t code);
  void op_op(cop2_state_t *state, uint32_t code);
  void op_rtps(cop2_state_t *state, uint32_t code);
  void op_rtpt(cop2_state_t *state, uint32_t code);
  void op_sqr(cop2_state_t *state, uint32_t code);

  namespace flags {
    enum {
      A1_MAX = 30,
      A2_MAX = 29,
      A3_MAX = 28,
      A1_MIN = 27,
      A2_MIN = 26,
      A3_MIN = 25,
      B1 = 24,
      B2 = 23,
      B3 = 22,
      C1 = 21,
      C2 = 20,
      C3 = 19,
      D = 18,
      E = 17,
      F_MAX = 16,
      F_MIN = 15,
      G1 = 14,
      G2 = 13,
      H = 12
    };

    int64_t a(cop2_state_t *state, int n, int64_t value);

    int32_t b(cop2_state_t *state, int n, uint32_t code, int32_t value);

    int32_t b(cop2_state_t *state, int n, uint32_t code, int32_t value, int32_t shifted);

    int32_t c(cop2_state_t *state, int n, int32_t value);

    int32_t d(cop2_state_t *state, int32_t value);

    int32_t e(cop2_state_t *state);

    int64_t f(cop2_state_t *state, int64_t value);

    int32_t g(cop2_state_t *state, int n, int32_t value);

    int32_t h(cop2_state_t *state, int64_t value);
  }
}

#endif //__PSXACT_CPU_COP2_HPP__
