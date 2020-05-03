#include "spu/adsr.hpp"

#include <cassert>
#include "util/uint.hpp"

using namespace psx::spu;
using namespace psx::util;

adsr_t::adsr_t()
  : state(adsr_state_t::attack)
  , params()
  , divider(0)
  , level(0)
  , level_sustain(0) {
  put_config_lo(0);
  put_config_hi(0);
}

void adsr_t::step() {
  auto &params = get_current_params();

  int div_step = params.get_div_step(level);
  divider += div_step;

  if (divider >= 0x8000) {
    divider = 0; // TODO: should this be `divider -= 0x8000`?

    int lev_step = params.get_lev_step(level);
    level = uint_t<15>::clamp(level + lev_step);

    if (state == adsr_state_t::attack && level == 0x7fff) {
      state = adsr_state_t::decay;
    } else if (state == adsr_state_t::decay && level <= level_sustain) {
      state = adsr_state_t::sustain;
    }
  }
}

void adsr_t::key_on() {
  divider = 0;
  level = 0;
  state = adsr_state_t::attack;
}

void adsr_t::key_off() {
  divider = 0;
  state = adsr_state_t::release;
}

auto adsr_t::get_level() const -> int16_t {
  return level;
}

auto adsr_t::put_level(int16_t val) -> void {
  level = val;
}

auto adsr_t::get_current_params() -> envelope_params_t& {
  return params[int32_t(state)];
}

void adsr_t::put_config_lo(uint16_t val) {
  auto attack_mode = envelope_mode_t((val >> 15) & 1);
  auto attack_shift = (val >> 10) & 31;
  auto attack_step = 7 - ((val >> 8) & 3);

  params[0].put_params(attack_mode, attack_shift, attack_step);

  if (attack_mode == envelope_mode_t::exponential) {
    params[0].put_multi_phase(attack_step);
  }

  auto decay_mode = envelope_mode_t::exponential;
  auto decay_shift = (val >> 4) & 15;
  auto decay_step = -8;

  params[1].put_params(decay_mode, decay_shift, decay_step);

  level_sustain = (((val & 15) + 1) << 11) - 1;
  assert(level_sustain < 0x8000);
}

void adsr_t::put_config_hi(uint16_t val) {
  auto sustain_mode = envelope_mode_t((val >> 15) & 1);
  auto sustain_direction = envelope_direction_t((val >> 14) & 1);
  auto sustain_shift = (val >> 8) & 31;
  auto sustain_step_raw = 7 - ((val >> 6) & 3);
  auto sustain_step = 0;

  if (sustain_direction == envelope_direction_t::decrease) {
    sustain_step = ~sustain_step_raw;
  } else {
    sustain_step = sustain_step_raw;
  }

  params[2].put_params(sustain_mode, sustain_shift, sustain_step);

  if (sustain_mode == envelope_mode_t::exponential && sustain_direction == envelope_direction_t::increase) {
    params[2].put_multi_phase(sustain_step_raw);
  }

  auto release_mode = envelope_mode_t((val >> 5) & 1);
  auto release_shift = val & 31;
  auto release_step = -8;

  params[3].put_params(release_mode, release_shift, release_step);
}
