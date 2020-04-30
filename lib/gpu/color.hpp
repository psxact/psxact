#ifndef GPU_COLOR_HPP_
#define GPU_COLOR_HPP_

#include <cstdint>

namespace psx::gpu {
  struct color_t {
    uint8_t r = {};
    uint8_t g = {};
    uint8_t b = {};

    static color_t from_uint16(uint16_t val);
    static color_t from_uint24(uint32_t val);

    uint16_t to_uint16() const;
  };
}

#endif  // GPU_COLOR_HPP_
