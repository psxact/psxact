#include "gpu/core.hpp"

#include "util/uint.hpp"

using namespace psx::gpu;
using namespace psx::util;

static const int dither_lut[4][4] = {
  { -4,  0, -3,  1 },
  {  2, -2,  3, -1 },
  { -3,  1, -4,  0 },
  {  3, -1,  2, -2 }
};

void core_t::draw_point(point_t point, color_t color) {
  if (point.x < drawing_area_x1 ||
      point.x > drawing_area_x2 ||
      point.y < drawing_area_y1 ||
      point.y > drawing_area_y2) {
    return;
  }

  auto dither = dither_lut[point.y & 3][point.x & 3];

  color.r = uint_t<8>::clamp(color.r + dither);
  color.g = uint_t<8>::clamp(color.g + dither);
  color.b = uint_t<8>::clamp(color.b + dither);

  vram_write(point.x, point.y, color_to_uint16(color));
}
