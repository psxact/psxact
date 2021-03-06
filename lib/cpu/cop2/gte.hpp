#ifndef CPU_COP2_GTE_HPP_
#define CPU_COP2_GTE_HPP_

#include <algorithm>
#include <cstdint>

#include "cpu/coprocessor.hpp"
#include "util/logger.hpp"

namespace psx::cpu::cop2 {

  enum class matrix {
    rot = 0,
    llm = 1,
    lcm = 2,
    nil = 3
  };

  enum class vector {
    tr = 0,
    bk = 1,
    fc = 2,
    zr = 3
  };

  union color {
    struct {
      uint8_t r;
      uint8_t g;
      uint8_t b;
      uint8_t c;
    };

    uint32_t value = {};
  };

  class gte : public coprocessor {
		psx::util::logger logger;

    struct {
      int32_t matrix[4][3][3] = {};
      int32_t vector[4][3] = {};
      int32_t ofx = {};
      int32_t ofy = {};
      int32_t h = {};
      int32_t dqa = {};
      int32_t dqb = {};
      int32_t zsf3 = {};
      int32_t zsf4 = {};
      uint32_t flag = {};
    } ccr = {};

    struct {
      int32_t vector[4][3] = {};
      color rgbc = {};
      int32_t otz = {};
      int32_t ir0 = {};
      int32_t sx[3] = {};
      int32_t sy[3] = {};
      int32_t sz[4] = {};
      color rgb[3] = {};
      int32_t res = {};
      int32_t mac[4] = {};
      int32_t lzcs = {};
      int32_t lzcr = {};
    } gpr = {};

  public:
		gte();

    void run(uint32_t code) override;

    uint32_t read_matrix_vector_group(uint32_t n);
    uint32_t read_ccr(uint32_t n) override;

    void write_matrix_vector_group(uint32_t n, uint32_t value);
    void write_ccr(uint32_t n, uint32_t value) override;

    uint32_t read_gpr(uint32_t n) override;
    void write_gpr(uint32_t n, uint32_t value) override;

    uint32_t divide();

  private:
    matrix get_mx(uint32_t code);
    vector get_cv(uint32_t code);

    // -============-
    //  Instructions
    // -============-

    void mac_to_ir(uint32_t code);
    void mac_to_rgb();

    void depth_cue(uint32_t code, int32_t r, int32_t g, int32_t b);

    void transform_dq(int64_t div);
    void transform_xy(int64_t div);

    int64_t transform(uint32_t code, matrix mx, vector cv, int32_t v);
    int64_t transform_buggy(uint32_t code, matrix mx, vector cv, int32_t v);
    int64_t transform_pt(uint32_t code, matrix mx, vector cv, int32_t v);

    void op_avsz3();
    void op_avsz4();
    void op_cc(uint32_t code);
    void op_cdp(uint32_t code);
    void op_dcpl(uint32_t code);
    void op_dpcs(uint32_t code);
    void op_dpct(uint32_t code);
    void op_gpf(uint32_t code);
    void op_gpl(uint32_t code);
    void op_intpl(uint32_t code);
    void op_mvmva(uint32_t code);
    void op_nccs(uint32_t code);
    void op_ncct(uint32_t code);
    void op_ncds(uint32_t code);
    void op_ncdt(uint32_t code);
    void op_nclip();
    void op_ncs(uint32_t code);
    void op_nct(uint32_t code);
    void op_op(uint32_t code);
    void op_rtps(uint32_t code);
    void op_rtpt(uint32_t code);
    void op_sqr(uint32_t code);

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

    void set_flag(int32_t flag);

    int64_t flag_a(int32_t n, int64_t value);
    int32_t flag_b(int32_t n, uint32_t code, int32_t value);
    int32_t flag_b(int32_t n, uint32_t code, int32_t value, int32_t shifted);
    int32_t flag_c(int32_t n, int32_t value);
    int32_t flag_d(int32_t value);
    int32_t flag_e();
    int64_t flag_f(int64_t value);
    int32_t flag_g(int32_t n, int32_t value);
    int32_t flag_h(int64_t value);
  };
}

#endif
