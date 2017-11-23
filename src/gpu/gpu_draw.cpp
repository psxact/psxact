#include "gpu.hpp"
#include "../limits.hpp"
#include "../memory/vram.hpp"


static const int dither_lut[4][4] = {
  { -4,  0, -3,  1 },
  {  2, -2,  3, -1 },
  { -3,  1, -4,  0 },
  {  3, -1,  2, -2 }
};


void gpu_t::draw_point(point_t point, color_t color) {
  if (point.x < drawing_area_x1 ||
      point.x > drawing_area_x2 ||
      point.y < drawing_area_y1 ||
      point.y > drawing_area_y2) {
    return;
  }

  auto dither = dither_lut[point.y & 3][point.x & 3];

  color.r = ulimit<8>::clamp(color.r + dither);
  color.g = ulimit<8>::clamp(color.g + dither);
  color.b = ulimit<8>::clamp(color.b + dither);

  vram::write(point.x, point.y, color_to_uint16(color));
}
