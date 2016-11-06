#include "gpu_core.hpp"

template<int min, int max>
int clip(int value) {
  if (value <= min) return min;
  if (value >= max) return max;
  return value;
};

static int dither_lut[4][4] = {
    { -4,  0, -3,  1 },
    {  2, -2,  3, -1 },
    { -3,  1, -4,  0 },
    {  3, -1,  2, -2 }
};

void gpu::draw_point(int x, int y, int r, int g, int b) {
  auto address = (y << 10) + x;

  auto dither = dither_lut[y & 3][x & 3];

  r = clip<0, 255>(r + dither);
  g = clip<0, 255>(g + dither);
  b = clip<0, 255>(b + dither);

  auto color =
      (((r >> 3) & 0x1f) <<  0) |
      (((g >> 3) & 0x1f) <<  5) |
      (((b >> 3) & 0x1f) << 10);

  gpu::vram.h[address] = uint16_t(color);
}
