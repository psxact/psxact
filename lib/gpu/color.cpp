#include "gpu/color.hpp"

using namespace psx::gpu;

color_t color_t::from_uint16(uint16_t val) {
  color_t color;
  color.r = (val << 3) & 0xf8;
  color.g = (val >> 2) & 0xf8;
  color.b = (val >> 7) & 0xf8;

  return color;
}

color_t color_t::from_uint24(uint32_t val) {
  color_t color;
  color.r = (val >> (8 * 0)) & 0xff;
  color.g = (val >> (8 * 1)) & 0xff;
  color.b = (val >> (8 * 2)) & 0xff;

  return color;
}

uint16_t color_t::to_uint16() const {
  return
    ((r >> 3) & 0x001f) |
    ((g << 2) & 0x03e0) |
    ((b << 7) & 0x7c00);
}
