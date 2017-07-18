#include "gpu_core.hpp"
#include "../memory/vram.hpp"

static const int dither_lut[4][4] = {
  { -4,  0, -3,  1 },
  {  2, -2,  3, -1 },
  { -3,  1, -4,  0 },
  {  3, -1,  2, -2 }
};

static uint16_t color_to_15bpp(gpu::color_t color) {
  return
    (((color.r >> 3) & 0x1f) << 0) |
    (((color.g >> 3) & 0x1f) << 5) |
    (((color.b >> 3) & 0x1f) << 10);
}

void gpu::draw_point(gpu_state_t *state, point_t point, color_t color) {
  if (point.x < int(state->drawing_area_x1) || point.x > int(state->drawing_area_x2)) return;
  if (point.y < int(state->drawing_area_y1) || point.y > int(state->drawing_area_y2)) return;

  auto dither = dither_lut[point.y & 3][point.x & 3];

  color.r = utility::uclamp<8>(color.r + dither);
  color.g = utility::uclamp<8>(color.g + dither);
  color.b = utility::uclamp<8>(color.b + dither);

  vram::write(point.x, point.y, color_to_15bpp(color));
}
