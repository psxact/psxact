#ifndef SPU_VOLUME_HPP_
#define SPU_VOLUME_HPP_

#include <cstdint>

namespace psx::spu {

  struct volume_t {
    bool fixed;
    int16_t level;

    int32_t apply(int32_t sample);

    static volume_t create(uint16_t val);
  };
}

#endif  // SPU_VOLUME_HPP_
