#ifndef SPU_ADSR_ENVELOPE_HPP_
#define SPU_ADSR_ENVELOPE_HPP_

#include <cstdint>
#include "spu/envelope.hpp"

namespace psx::spu {

  enum class adsr_state {
    attack,
    decay,
    sustain,
    release
  };

  class adsr_envelope {
    adsr_state state;
    envelope_params params[4];

    uint16_t divider;

    int16_t level;
    int16_t level_sustain;

  public:
    adsr_envelope();

    void step();

    void key_on();
    void key_off();

    auto get_level() const -> int16_t;
    void put_level(int16_t val);

    auto get_current_params() -> envelope_params&;

    void put_config_lo(uint16_t val);
    void put_config_hi(uint16_t val);
  };
}

#endif
