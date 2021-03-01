#include "envelope.hpp"

#include "util/panic.hpp"
#include "util/uint.hpp"

using namespace psx::spu;
using namespace psx::util;

envelope_params::envelope_params()
  : mode(envelope_mode::linear)
  , div_step(0)
  , lev_step(0) {}

auto envelope_params::get_div_step(int16_t lev) -> uint16_t {
  if (mode == envelope_mode::multi_phase && lev >= 0x6000) {
    return div_step_multi_phase;
  } else {
    return div_step;
  }
}

auto envelope_params::get_lev_step(int16_t lev) -> int16_t {
  switch (mode) {
    case envelope_mode::linear: {
      return lev_step;
    }

    case envelope_mode::exponential: {
      int32_t s = int32_t(lev_step);
      int32_t l = int32_t(lev);

      return int16_t((s * l) >> 15);
    }

    case envelope_mode::multi_phase: {
      return lev >= 0x6000
        ? lev_step_multi_phase
        : lev_step;
    }

    default:
      PANIC("Unexpected value for envelope_mode.");
      return 0;
  }
}

void envelope_params::put_params(envelope_mode envelope_mode, int32_t shift, int16_t step) {
  constexpr int time = 0x8000;

  mode = envelope_mode;
  lev_step = step << uint_t<4>::clamp(11 - shift);
  div_step = time >> uint_t<4>::clamp(shift - 11);
}

void envelope_params::put_multi_phase(int16_t step) {
  mode = envelope_mode::multi_phase;

  if (step > 10 && div_step > 3) {
    div_step_multi_phase = div_step >> 2;
  } else if (step >= 10 && div_step > 1) {
    div_step_multi_phase = div_step >> 1;
  } else {
    div_step_multi_phase = div_step;
  }

  if (div_step_multi_phase == 0) {
    div_step_multi_phase = 1;
  }

  if (step < 10) {
    lev_step_multi_phase = lev_step >> 2;
  } else if (step == 10) {
    lev_step_multi_phase = lev_step >> 1;
  } else {
    lev_step_multi_phase = lev_step;
  }
}
