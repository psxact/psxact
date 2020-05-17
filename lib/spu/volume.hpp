#ifndef SPU_VOLUME_HPP_
#define SPU_VOLUME_HPP_

#include <cstdint>
#include "spu/envelope.hpp"

namespace psx::spu {

  enum class volume_mode_t {
    fixed,
    sweep
  };

  struct volume_t {
    envelope_params_t params;
    volume_mode_t mode;
    int16_t level;

    int16_t get_level();
    void put_level(uint16_t val);
  };
}

#endif
