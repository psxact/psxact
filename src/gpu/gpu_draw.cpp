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

void draw_point(const int &x, const int &y, const int &r, const int &g, const int &b) {
  auto address = (y << 10) + x;

  auto dither = dither_lut[y & 3][x & 3];

  auto r_ = clip<0, 255>(r + dither);
  auto g_ = clip<0, 255>(g + dither);
  auto b_ = clip<0, 255>(b + dither);

  auto color =
      (((r_ >> 3) & 0x1f) <<  0) |
      (((g_ >> 3) & 0x1f) <<  5) |
      (((b_ >> 3) & 0x1f) << 10);

  gpu::vram.h[address] = uint16_t(color);
}

static int min3(int a, int b, int c) {
  if (a <= b && a <= c) return a;
  if (b <= a && b <= c) return b;
  return c;
}

static int max3(int a, int b, int c) {
  if (a >= b && a >= c) return a;
  if (b >= a && b >= c) return b;
  return c;
}

static int edge(const gpu::point_t& a, const gpu::point_t& b, const gpu::point_t& c) {
  return ((b.x - a.x) * (c.y - a.y)) - ((b.y - a.y) * (c.x - a.x));
}

static bool is_top_left(const gpu::point_t &a, const gpu::point_t &b) {
  return (b.y == a.y && b.x > a.x) || b.y < a.y;
}

static void draw_poly3_(const gpu::point_t &v0, const gpu::point_t &v1, const gpu::point_t &v2) {
  int min_x = min3(v0.x, v1.x, v2.x);
  int min_y = min3(v0.y, v1.y, v2.y);
  int max_x = max3(v0.x, v1.x, v2.x);
  int max_y = max3(v0.y, v1.y, v2.y);

  bool is_top_left_12 = is_top_left(v1, v2);
  bool is_top_left_20 = is_top_left(v2, v0);
  bool is_top_left_01 = is_top_left(v0, v1);

  int A01 = v0.y - v1.y, B01 = v1.x - v0.x;
  int A12 = v1.y - v2.y, B12 = v2.x - v1.x;
  int A20 = v2.y - v0.y, B20 = v0.x - v2.x;

  gpu::point_t p = { min_x, min_y };
  int w0_row = edge(v1, v2, p);
  int w1_row = edge(v2, v0, p);
  int w2_row = edge(v0, v1, p);

  for (p.y = min_y; p.y <= max_y; p.y++) {
    int w0 = w0_row;
    int w1 = w1_row;
    int w2 = w2_row;

    for (p.x = min_x; p.x <= max_x; p.x++) {
      bool draw =
          (w0 > 0 || (w0 == 0 && is_top_left_12)) &&
          (w1 > 0 || (w1 == 0 && is_top_left_20)) &&
          (w2 > 0 || (w2 == 0 && is_top_left_01));

      if (draw) {
        auto r = ((v0.r * w0) + (v1.r * w1) + (v2.r * w2)) / (w0 + w1 + w2);
        auto g = ((v0.g * w0) + (v1.g * w1) + (v2.g * w2)) / (w0 + w1 + w2);
        auto b = ((v0.b * w0) + (v1.b * w1) + (v2.b * w2)) / (w0 + w1 + w2);

        draw_point(p.x, p.y, r, g, b);
      }

      w0 += A12;
      w1 += A20;
      w2 += A01;
    }

    w0_row += B12;
    w1_row += B20;
    w2_row += B01;
  }
}

static inline int double_area(const gpu::point_t &v0, const gpu::point_t &v1, const gpu::point_t &v2) {
  auto e0 = (v1.x - v0.x) * (v1.y + v0.y);
  auto e1 = (v2.x - v1.x) * (v2.y + v1.y);
  auto e2 = (v0.x - v2.x) * (v0.y + v2.y);

  return e0 + e1 + e2;
}

void gpu::draw_poly3(const gpu::point_t &v0, const gpu::point_t &v1, const gpu::point_t &v2) {
  if (double_area(v0, v1, v2) < 0) {
    draw_poly3_(v0, v1, v2);
  } else {
    draw_poly3_(v0, v2, v1);
  }
}

void gpu::draw_poly4(const gpu::point_t &v0, const gpu::point_t &v1, const gpu::point_t &v2, const gpu::point_t &v3) {
  gpu::draw_poly3(v0, v1, v2);
  gpu::draw_poly3(v1, v2, v3);
}
