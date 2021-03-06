#ifndef SPU_ENVELOPE_HPP_
#define SPU_ENVELOPE_HPP_

#include <cstdint>

namespace psx::spu {

  enum class envelope_mode {
    linear,
    exponential,
    multi_phase
  };

  enum class envelope_direction {
    increase,
    decrease
  };

  struct envelope_params {
    envelope_mode mode;

    uint16_t div_step;
    int16_t lev_step;

    uint16_t div_step_multi_phase;
    int16_t lev_step_multi_phase;

    envelope_params();

    auto get_div_step(int16_t level) -> uint16_t;
    auto get_lev_step(int16_t level) -> int16_t;

    void put_params(envelope_mode envelope_mode, int32_t shift, int16_t step);
    void put_multi_phase(int16_t step);
  };
}

#endif
