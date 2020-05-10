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
    uint32_t to_uint32() const;
  };

  /// Used to represent a color that has come from a texture.
  struct texture_color_t {
    uint16_t value;

    color_t to_color() const;
    bool is_semi_transparent() const;
    bool is_full_transparent() const;
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

  class gp0_command_t final {
    uint32_t command;

  public:
    gp0_command_t(uint32_t command) : command(command) {}

    /// Returns true if bit 24 of `command` is set, otherwise false.
    bool is_raw_texture() const;

    /// Returns true if bit 25 of `command` is set, otherwise false.
    bool is_semi_transparent() const;

    /// Returns true if bit 26 of `command` is set, otherwise false.
    bool is_texture_mapped() const;

    /// Returns true if bit 27 of `command` is set, otherwise false.
    bool is_poly_line() const;

    /// Returns true if bit 27 of `command` is set, otherwise false.
    bool is_quad_poly() const;

    /// Returns true if bit 28 of `command` is set, otherwise false.
    bool is_gouraud_shaded() const;

    /*
      27-28 Rect Size   (0=Var, 1=1x1, 2=8x8, 3=16x16) (Rectangle only)
      29-31 Primitive Type    (1=Polygon, 2=Line, 3=Rectangle)
    */
  };
}

#endif  // GPU_TYPES_HPP_
