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

static int get_min_point(const gpu::triangle_t &tri) {
  if (tri.v[0].y < tri.v[1].y && tri.v[0].y < tri.v[2].y) { return 0; }
  if (tri.v[1].y < tri.v[0].y && tri.v[1].y < tri.v[2].y) { return 1; }
  return 2;
}

static int get_max_point(const gpu::triangle_t &tri) {
  if (tri.v[0].y > tri.v[1].y && tri.v[0].y > tri.v[2].y) { return 0; }
  if (tri.v[1].y > tri.v[0].y && tri.v[1].y > tri.v[2].y) { return 1; }
  return 2;
}

constexpr int precision = 8;

void draw_span(int y, int x1, int r1, int g1, int b1, int x2, int r2, int g2, int b2) {
  if (x1 == x2) {
    draw_point(x1, y, r1, g1, b1);
    return;
  }

  if (x1 < x2) {
    int dr = ((r2 - r1) << precision) / (x2 - x1);
    int dg = ((g2 - g1) << precision) / (x2 - x1);
    int db = ((b2 - b1) << precision) / (x2 - x1);

    int r = r1 << precision;
    int g = g1 << precision;
    int b = b1 << precision;

    for (int x = x1; x <= x2; x++) {
      draw_point(x,
                 y,
                 r >> precision,
                 g >> precision,
                 b >> precision);

      r += dr;
      g += dg;
      b += db;
    }
  } else {
    int dr = ((r1 - r2) << precision) / (x1 - x2);
    int dg = ((g1 - g2) << precision) / (x1 - x2);
    int db = ((b1 - b2) << precision) / (x1 - x2);

    int r = r2 << precision;
    int g = g2 << precision;
    int b = b2 << precision;

    for (int x = x2; x <= x1; x++) {
      draw_point(x,
                 y,
                 r >> precision,
                 g >> precision,
                 b >> precision);

      r += dr;
      g += dg;
      b += db;
    }
  }
}

void draw_upper_triangle(const gpu::point_t &v1, const gpu::point_t &v2, const gpu::point_t &v3) {
  auto dx1 = ((v2.x - v1.x) << precision) / (v2.y - v1.y);
  auto dr1 = ((v2.r - v1.r) << precision) / (v2.y - v1.y);
  auto dg1 = ((v2.g - v1.g) << precision) / (v2.y - v1.y);
  auto db1 = ((v2.b - v1.b) << precision) / (v2.y - v1.y);

  auto x1 = v1.x << precision;
  auto r1 = v1.r << precision;
  auto g1 = v1.g << precision;
  auto b1 = v1.b << precision;

  auto dx2 = ((v3.x - v1.x) << precision) / (v3.y - v1.y);
  auto dr2 = ((v3.r - v1.r) << precision) / (v3.y - v1.y);
  auto dg2 = ((v3.g - v1.g) << precision) / (v3.y - v1.y);
  auto db2 = ((v3.b - v1.b) << precision) / (v3.y - v1.y);

  auto x2 = v1.x << precision;
  auto r2 = v1.r << precision;
  auto g2 = v1.g << precision;
  auto b2 = v1.b << precision;

  for (int y = v1.y; y <= v2.y; y++) {
    draw_span(y,
              x1 >> precision,
              r1 >> precision,
              g1 >> precision,
              b1 >> precision,
              x2 >> precision,
              r2 >> precision,
              g2 >> precision,
              b2 >> precision);

    x1 += dx1;
    r1 += dr1;
    g1 += dg1;
    b1 += db1;

    x2 += dx2;
    r2 += dr2;
    g2 += dg2;
    b2 += db2;
  }
}

void draw_lower_triangle(const gpu::point_t &v1, const gpu::point_t &v2, const gpu::point_t &v3) {
  auto dx1 = ((v3.x - v1.x) << precision) / (v3.y - v1.y);
  auto dr1 = ((v3.r - v1.r) << precision) / (v3.y - v1.y);
  auto dg1 = ((v3.g - v1.g) << precision) / (v3.y - v1.y);
  auto db1 = ((v3.b - v1.b) << precision) / (v3.y - v1.y);

  auto x1 = v3.x << precision;
  auto r1 = v3.r << precision;
  auto g1 = v3.g << precision;
  auto b1 = v3.b << precision;

  auto dx2 = ((v3.x - v2.x) << precision) / (v3.y - v2.y);
  auto dr2 = ((v3.r - v2.r) << precision) / (v3.y - v2.y);
  auto dg2 = ((v3.g - v2.g) << precision) / (v3.y - v2.y);
  auto db2 = ((v3.b - v2.b) << precision) / (v3.y - v2.y);

  auto x2 = v3.x << precision;
  auto r2 = v3.r << precision;
  auto g2 = v3.g << precision;
  auto b2 = v3.b << precision;

  for (int y = v3.y; y > v1.y; y--) {
    draw_span(y,
              x1 >> precision,
              r1 >> precision,
              g1 >> precision,
              b1 >> precision,
              x2 >> precision,
              r2 >> precision,
              g2 >> precision,
              b2 >> precision);

    x1 -= dx1;
    r1 -= dr1;
    g1 -= dg1;
    b1 -= db1;

    x2 -= dx2;
    r2 -= dr2;
    g2 -= dg2;
    b2 -= db2;
  }
}

void gpu::draw_triangle(const gpu::triangle_t &tri) {
  // sort the points by y-value
  //

  int v1_point = get_min_point(tri);
  int v3_point = get_max_point(tri);
  int v2_point = 3 - (v1_point + v3_point);

  const auto &v1 = tri.v[v1_point];
  const auto &v2 = tri.v[v2_point];
  const auto &v3 = tri.v[v3_point];

  if (v2.y == v3.y) {
    return draw_upper_triangle(v1, v2, v3);
  }

  if (v2.y == v1.y) {
    return draw_lower_triangle(v1, v2, v3);
  }

  if (v3.y == v1.y) {
    return;
  }

  gpu::point_t v4;
  v4.x = v1.x + (((v2.y - v1.y) * (v3.x - v1.x)) / (v3.y - v1.y));
  v4.y = v2.y;
  v4.r = v3.r + (((v2.y - v1.y) * (v3.r - v1.r)) / (v3.y - v1.y));
  v4.g = v3.g + (((v2.y - v1.y) * (v3.g - v1.g)) / (v3.y - v1.y));
  v4.b = v3.b + (((v2.y - v1.y) * (v3.b - v1.b)) / (v3.y - v1.y));

  draw_upper_triangle(v1, v2, v4);
  draw_lower_triangle(v2, v4, v3);
}

void gpu::draw_quad(const gpu::quad_t &quad) {
  gpu::draw_triangle({ quad.v0, quad.v2, quad.v1 });
  gpu::draw_triangle({ quad.v1, quad.v2, quad.v3 });
}
