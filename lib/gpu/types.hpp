#ifndef GPU_TYPES_HPP_
#define GPU_TYPES_HPP_

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

  struct point_t {
    int32_t x = {};
    int32_t y = {};

    static point_t from_uint24(uint32_t val);
  };

  struct texture_coord_t {
    uint8_t u = {};
    uint8_t v = {};

    static texture_coord_t from_uint16(uint16_t val);
  };

  struct tev_t {
    // from 'palette'
    int32_t palette_page_x = {};
    int32_t palette_page_y = {};

    // from 'texpage'
    int32_t texture_colors = {};
    int32_t texture_page_x = {};
    int32_t texture_page_y = {};
    int32_t color_mix_mode = {};
  };

  struct triangle_t {
    color_t colors[3] = {};
    texture_coord_t coords[3] = {};
    point_t points[3] = {};

    tev_t tev = {};
  };
}

#endif  // GPU_TYPES_HPP_
