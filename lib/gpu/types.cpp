#include "gpu/types.hpp"

#include "util/int.hpp"
#include "util/uint.hpp"

using namespace psx::gpu;
using namespace psx::util;

// color_t

color_t color_t::from_uint16(uint16_t val) {
  color_t color;
  color.r = (val << 3) & 0xf8;
  color.g = (val >> 2) & 0xf8;
  color.b = (val >> 7) & 0xf8;

  return color;
}

color_t color_t::from_uint24(uint32_t val) {
  color_t color;
  color.r = uint_t<8>::trunc(val);
  color.g = uint_t<8>::trunc(val);
  color.b = uint_t<8>::trunc(val);

  return color;
}

uint16_t color_t::to_uint16() const {
  return
    ((r >> 3) & 0x001f) |
    ((g << 2) & 0x03e0) |
    ((b << 7) & 0x7c00);
}

uint32_t color_t::to_uint32() const {
  return (r << 16) | (g << 8) | b;
}

// point_t

point_t point_t::from_uint24(uint32_t val) {
  point_t point;
  point.x = int_t<11>::trunc(val);
  point.y = int_t<11>::trunc(val >> 16);

  return point;
}

// texture_coord_t

texture_coord_t texture_coord_t::from_uint16(uint16_t val) {
  texture_coord_t coord;
  coord.u = uint_t<8>::trunc(val);
  coord.v = uint_t<8>::trunc(val >> 8);

  return coord;
}

// gp0_command_t

bool gp0_command_t::is_raw_texture() const {
  return (command & (1 << 24)) != 0;
}

bool gp0_command_t::is_semi_transparent() const {
  return (command & (1 << 25)) != 0;
}

bool gp0_command_t::is_texture_mapped() const {
  return (command & (1 << 26)) != 0;
}

bool gp0_command_t::is_poly_line() const {
  return (command & (1 << 27)) != 0;
}

bool gp0_command_t::is_quad_poly() const {
  return (command & (1 << 27)) != 0;
}

bool gp0_command_t::is_gouraud_shaded() const {
  return (command & (1 << 28)) != 0;
}
