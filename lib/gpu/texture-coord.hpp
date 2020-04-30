#ifndef GPU_TEXTURE_COORD_HPP_
#define GPU_TEXTURE_COORD_HPP_

#include <cstdint>

namespace psx::gpu {
  struct texture_coord_t {
    uint8_t u = {};
    uint8_t v = {};

    static texture_coord_t from_uint16(uint16_t val);
  };
}

#endif  // GPU_TEXTURE_COORD_HPP_
