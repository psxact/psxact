#include "gpu/types.hpp"

#include "util/int.hpp"
#include "util/uint.hpp"

using namespace psx::gpu;
using namespace psx::util;

// texture_color

color texture_color::to_color() const {
  return color::from_uint16(value);
}

bool texture_color::is_semi_transparent() const {
  return (value & 0x8000) != 0;
}

bool texture_color::is_full_transparent() const {
  return value == 0;
}

// color

color color::from_uint16(uint16_t val) {
  color color;
  color.r = (val << 3) & 0xf8;
  color.g = (val >> 2) & 0xf8;
  color.b = (val >> 7) & 0xf8;

  return color;
}

color color::from_uint24(uint32_t val) {
  color color;
  color.r = uint_t<8>::trunc(val);
  color.g = uint_t<8>::trunc(val >> 8);
  color.b = uint_t<8>::trunc(val >> 16);

  return color;
}

uint16_t color::to_uint16() const {
  return
    ((r >> 3) & 0x001f) |
    ((g << 2) & 0x03e0) |
    ((b << 7) & 0x7c00);
}

uint32_t color::to_uint32() const {
  return (r << 16) | (g << 8) | b;
}

// point

point point::from_uint24(uint32_t val) {
  point point;
  point.x = int_t<11>::trunc(val);
  point.y = int_t<11>::trunc(val >> 16);

  return point;
}

// texture_coord

texture_coord texture_coord::from_uint16(uint16_t val) {
  texture_coord coord;
  coord.u = uint_t<8>::trunc(val);
  coord.v = uint_t<8>::trunc(val >> 8);

  return coord;
}

// gp0_command

bool gp0_command::is_raw_texture() const {
  return (command & (1 << 24)) != 0;
}

bool gp0_command::is_semi_transparent() const {
  return (command & (1 << 25)) != 0;
}

bool gp0_command::is_texture_mapped() const {
  return (command & (1 << 26)) != 0;
}

bool gp0_command::is_poly_line() const {
  return (command & (1 << 27)) != 0;
}

bool gp0_command::is_quad_poly() const {
  return (command & (1 << 27)) != 0;
}

bool gp0_command::is_gouraud_shaded() const {
  return (command & (1 << 28)) != 0;
}
