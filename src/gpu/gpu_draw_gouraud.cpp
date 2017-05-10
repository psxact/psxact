#include "gpu_core.hpp"

void fill_gouraud(
  gpu_state_t *state,
  const gpu::gouraud::pixel_t &v0, const int &w0,
  const gpu::gouraud::pixel_t &v1, const int &w1,
  const gpu::gouraud::pixel_t &v2, const int &w2, int x, int y) {
  int area = w0 + w1 + w2;

  int r = ((v0.color.r * w0) + (v1.color.r * w1) + (v2.color.r * w2)) / area;
  int g = ((v0.color.g * w0) + (v1.color.g * w1) + (v2.color.g * w2)) / area;
  int b = ((v0.color.b * w0) + (v1.color.b * w1) + (v2.color.b * w2)) / area;

  gpu::draw_point(state, x, y, r, g, b);
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

static int edge(
  const gpu::point_t& a,
  const gpu::point_t& b,
  const gpu::point_t& c) {
  return ((b.x - a.x) * (c.y - a.y)) - ((b.y - a.y) * (c.x - a.x));
}

static bool is_top_left(
  const gpu::point_t &a,
  const gpu::point_t &b) {
  return (b.y == a.y && b.x > a.x) || b.y < a.y;
}

static void fill_poly3_gouraud(
  gpu_state_t *state,
  const gpu::gouraud::pixel_t &v0,
  const gpu::gouraud::pixel_t &v1,
  const gpu::gouraud::pixel_t &v2) {
  int min_x = min3(v0.point.x, v1.point.x, v2.point.x);
  int min_y = min3(v0.point.y, v1.point.y, v2.point.y);
  int max_x = max3(v0.point.x, v1.point.x, v2.point.x);
  int max_y = max3(v0.point.y, v1.point.y, v2.point.y);

  bool is_top_left_12 = is_top_left(v1.point, v2.point);
  bool is_top_left_20 = is_top_left(v2.point, v0.point);
  bool is_top_left_01 = is_top_left(v0.point, v1.point);

  int x01 = v0.point.y - v1.point.y, y01 = v1.point.x - v0.point.x;
  int x12 = v1.point.y - v2.point.y, y12 = v2.point.x - v1.point.x;
  int x20 = v2.point.y - v0.point.y, y20 = v0.point.x - v2.point.x;

  gpu::point_t p = { min_x, min_y };

  int w0_row = edge(v1.point, v2.point, p);
  int w1_row = edge(v2.point, v0.point, p);
  int w2_row = edge(v0.point, v1.point, p);

  for (p.y = min_y; p.y <= max_y; p.y++) {
    int w0 = w0_row;
    w0_row += y12;

    int w1 = w1_row;
    w1_row += y20;

    int w2 = w2_row;
    w2_row += y01;

    for (p.x = min_x; p.x <= max_x; p.x++) {
      bool draw =
        (w0 > 0 || (w0 == 0 && is_top_left_12)) &&
        (w1 > 0 || (w1 == 0 && is_top_left_20)) &&
        (w2 > 0 || (w2 == 0 && is_top_left_01));

      if (draw) {
        fill_gouraud(state, v0, w0, v1, w1, v2, w2, p.x, p.y);
      }

      w0 += x12;
      w1 += x20;
      w2 += x01;
    }
  }
}

static inline int double_area(
  const gpu::point_t &v0,
  const gpu::point_t &v1,
  const gpu::point_t &v2) {
  auto e0 = (v1.x - v0.x) * (v1.y + v0.y);
  auto e1 = (v2.x - v1.x) * (v2.y + v1.y);
  auto e2 = (v0.x - v2.x) * (v0.y + v2.y);

  return e0 + e1 + e2;
}

void gpu::gouraud::draw_poly3(gpu_state_t *state, const polygon_t<3> &p) {
  const auto &p0 = p.v[0];
  const auto &p1 = p.v[1];
  const auto &p2 = p.v[2];

  if (double_area(p0.point, p1.point, p2.point) < 0) {
    fill_poly3_gouraud(state, p0, p1, p2);
  }
  else {
    fill_poly3_gouraud(state, p0, p2, p1);
  }
}

void gpu::gouraud::draw_poly4(gpu_state_t *state, const polygon_t<4> &p) {
  auto &v0 = p.v[0];
  auto &v1 = p.v[1];
  auto &v2 = p.v[2];
  auto &v3 = p.v[3];

  draw_poly3(state, { v0, v1, v2 });
  draw_poly3(state, { v1, v2, v3 });
}
