#include "gpu_core.hpp"

void fill_texture(const gpu::texture::poly3_t &p,
                  const int &w0,
                  const int &w1,
                  const int &w2, int x, int y) {
  int area = w0 + w1 + w2;

  int u = ((p.v0.u * w0) + (p.v1.u * w1) + (p.v2.u * w2)) / area;
  int v = ((p.v0.v * w0) + (p.v1.v * w1) + (p.v2.v * w2)) / area;

  auto texel = gpu::read_vram(p.base_u + (u / 4), p.base_v + v);
  int index = 0;

  switch (u & 3) {
    case 0: index = (texel >>  0) & 0xf; break;
    case 1: index = (texel >>  4) & 0xf; break;
    case 2: index = (texel >>  8) & 0xf; break;
    case 3: index = (texel >> 12) & 0xf; break;
  }

  auto color = gpu::read_vram(p.clut_x + index, p.clut_y);
  if (color == 0) {
    return;
  }

  gpu::write_vram(x, y, color);
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

static void fill_poly3_texture(const gpu::texture::poly3_t &t) {
  int min_x = min3(t.v0.point.x, t.v1.point.x, t.v2.point.x);
  int min_y = min3(t.v0.point.y, t.v1.point.y, t.v2.point.y);
  int max_x = max3(t.v0.point.x, t.v1.point.x, t.v2.point.x);
  int max_y = max3(t.v0.point.y, t.v1.point.y, t.v2.point.y);

  bool is_top_left_12 = is_top_left(t.v1.point, t.v2.point);
  bool is_top_left_20 = is_top_left(t.v2.point, t.v0.point);
  bool is_top_left_01 = is_top_left(t.v0.point, t.v1.point);

  int x01 = t.v0.point.y - t.v1.point.y, y01 = t.v1.point.x - t.v0.point.x;
  int x12 = t.v1.point.y - t.v2.point.y, y12 = t.v2.point.x - t.v1.point.x;
  int x20 = t.v2.point.y - t.v0.point.y, y20 = t.v0.point.x - t.v2.point.x;

  gpu::point_t p = { min_x, min_y };

  int w0_row = edge(t.v1.point, t.v2.point, p);
  int w1_row = edge(t.v2.point, t.v0.point, p);
  int w2_row = edge(t.v0.point, t.v1.point, p);

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
        fill_texture(t, w0, w1, w2, p.x, p.y);
      }

      w0 += x12;
      w1 += x20;
      w2 += x01;
    }

    w0_row += y12;
    w1_row += y20;
    w2_row += y01;
  }
}

static inline int double_area(const gpu::point_t &v0, const gpu::point_t &v1, const gpu::point_t &v2) {
  auto e0 = (v1.x - v0.x) * (v1.y + v0.y);
  auto e1 = (v2.x - v1.x) * (v2.y + v1.y);
  auto e2 = (v0.x - v2.x) * (v0.y + v2.y);

  return e0 + e1 + e2;
}

void gpu::texture::draw_poly3(const gpu::texture::poly3_t &p) {
  auto &v0 = p.v0;
  auto &v1 = p.v1;
  auto &v2 = p.v2;

  if (double_area(v0.point, v1.point, v2.point) < 0) {
    fill_poly3_texture({v0, v1, v2, p.clut_x, p.clut_y, p.base_u, p.base_v});
  } else {
    fill_poly3_texture({v0, v2, v1, p.clut_x, p.clut_y, p.base_u, p.base_v});
  }
}

void gpu::texture::draw_poly4(const gpu::texture::poly4_t &p) {
  auto &v0 = p.v0;
  auto &v1 = p.v1;
  auto &v2 = p.v2;
  auto &v3 = p.v3;

  gpu::texture::draw_poly3({v0, v1, v2, p.clut_x, p.clut_y, p.base_u, p.base_v});
  gpu::texture::draw_poly3({v1, v2, v3, p.clut_x, p.clut_y, p.base_u, p.base_v});
}
