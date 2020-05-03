#pragma once

#include <cstdint>

#include "spu/envelope.hpp"

namespace psx::spu {

  enum class adsr_state_t {
    attack,
    decay,
    sustain,
    release
  };

  class adsr_t {
    adsr_state_t state;
    envelope_params_t params[4];

    uint16_t divider;

    int16_t level;
    int16_t level_sustain;

  public:
    adsr_t();

    void step();

    void key_on();
    void key_off();

    auto get_level() const -> int16_t;
    void put_level(int16_t val);

    auto get_current_params() -> envelope_params_t&;

    void put_config_lo(uint16_t val);
    void put_config_hi(uint16_t val);
  };
}
