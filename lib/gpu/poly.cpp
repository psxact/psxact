#include "gpu/core.hpp"

#include <algorithm>
#include "util/int.hpp"
#include "util/uint.hpp"

using namespace psx::gpu;
using namespace psx::util;

static const int32_t point_factor_lut[4] = {
  1, 2, 2, 3
};

static const int32_t color_factor_lut[4] = {
  0, 0, 2, 3
};

static const int32_t coord_factor_lut[4] = {
  0, 2, 0, 3
};

static int32_t get_factor(const int32_t (&lut)[4], gp0_command command) {
  int32_t bit1 = command.is_gouraud_shaded();
  int32_t bit0 = command.is_texture_mapped();

  return lut[(bit1 << 1) | bit0];
}

static int32_t get_color_factor(gp0_command command) {
  return get_factor(color_factor_lut, command);
}

static int32_t get_point_factor(gp0_command command) {
  return get_factor(point_factor_lut, command);
}

static int32_t get_coord_factor(gp0_command command) {
  return get_factor(coord_factor_lut, command);
}

static void get_colors(const core &core, gp0_command command, color *colors, int32_t n) {
  int32_t factor = get_color_factor(command);

  for (int32_t i = 0; i < n; i++) {
    colors[i] = color::from_uint24(core.fifo.at(i * factor + 0));
  }
}

static void get_points(const core &core, gp0_command command, point *points, int32_t n) {
  int32_t factor = get_point_factor(command);

  for (int32_t i = 0; i < n; i++) {
    points[i] = point::from_uint24(core.fifo.at(i * factor + 1));
    points[i].x += core.x_offset;
    points[i].y += core.y_offset;
  }
}

static void get_coords(const core &core, gp0_command command, texture_coord *coords, int32_t n) {
  int32_t factor = get_coord_factor(command);

  for (int32_t i = 0; i < n; i++) {
    coords[i] = texture_coord::from_uint16(core.fifo.at(i * factor + 2));
  }
}

static texture_params get_tev(const core &core, gp0_command command) {
  int32_t factor = get_coord_factor(command);

  uint32_t palette = core.fifo.at(0 * factor + 2) >> 16;
  uint32_t texpage = core.fifo.at(1 * factor + 2) >> 16;

  texture_params result;

  //  11    Texture Disable (0=Normal, 1=Disable if GP1(09h).Bit0=1)   ;GPUSTAT.15

  result.palette_page_x = (palette << 4) & 0x3f0;
  result.palette_page_y = (palette >> 6) & 0x1ff;
  result.texture_colors = (texpage >> 7) & 3;
  result.texture_page_x = (texpage << 6) & 0x3c0;
  result.texture_page_y = (texpage << 4) & 0x100;

  if (command.is_texture_mapped()) {
    result.color_mix_mode = (texpage >> 5) & 3;
  } else {
    result.color_mix_mode = (core.get_status() >> 5) & 3;
  }

  return result;
}

static bool is_clockwise(const point *p) {
  int32_t sum =
    (p[1].x - p[0].x) * (p[1].y + p[0].y) +
    (p[2].x - p[1].x) * (p[2].y + p[1].y) +
    (p[0].x - p[2].x) * (p[0].y + p[2].y);

  return sum >= 0;
}

static int32_t edge_function(const point &a, const point &b, const point &c) {
  return
    ((a.x - b.x) * (c.y - b.y)) -
    ((a.y - b.y) * (c.x - b.x));
}

static int lerp(int a, int b, int c, int w0, int w1, int w2) {
  return ((w0 * a) + (w1 * b) + (w2 * c)) / (w0 + w1 + w2);
}

static color color_lerp(const color *c, int32_t w0, int32_t w1, int32_t w2) {
  color color;
  color.r = lerp(c[0].r, c[1].r, c[2].r, w0, w1, w2);
  color.g = lerp(c[0].g, c[1].g, c[2].g, w0, w1, w2);
  color.b = lerp(c[0].b, c[1].b, c[2].b, w0, w1, w2);

  return color;
}

static texture_coord point_lerp(const texture_coord *t, int32_t w0, int32_t w1, int32_t w2) {
  texture_coord point;
  point.u = lerp(t[0].u, t[1].u, t[2].u, w0, w1, w2);
  point.v = lerp(t[0].v, t[1].v, t[2].v, w0, w1, w2);

  return point;
}

void core::draw_triangle(gp0_command command, const triangle &triangle) {
  const point *v = triangle.points;

  point min;
  min.x = std::min(v[0].x, std::min(v[1].x, v[2].x));
  min.y = std::min(v[0].y, std::min(v[1].y, v[2].y));

  point max;
  max.x = std::max(v[0].x, std::max(v[1].x, v[2].x));
  max.y = std::max(v[0].y, std::max(v[1].y, v[2].y));

  min.x = std::max(min.x, drawing_area_x1);
  min.y = std::max(min.y, drawing_area_y1);
  max.x = std::min(max.x, drawing_area_x2);
  max.y = std::min(max.y, drawing_area_y2);

  int32_t dx[3];
  dx[0] = v[2].y - v[1].y;
  dx[1] = v[0].y - v[2].y;
  dx[2] = v[1].y - v[0].y;

  int32_t dy[3];
  dy[0] = v[1].x - v[2].x;
  dy[1] = v[2].x - v[0].x;
  dy[2] = v[0].x - v[1].x;

  int32_t c[3];
  c[0] = (dy[0] > 0 || (dy[0] == 0 && dx[0] > 0)) ? (-1) : 0;
  c[1] = (dy[1] > 0 || (dy[1] == 0 && dx[1] > 0)) ? (-1) : 0;
  c[2] = (dy[2] > 0 || (dy[2] == 0 && dx[2] > 0)) ? (-1) : 0;

  int32_t row[3];
  row[0] = edge_function(min, v[1], v[2]);
  row[1] = edge_function(min, v[2], v[0]);
  row[2] = edge_function(min, v[0], v[1]);

  point point;

  for (point.y = min.y; point.y <= max.y; point.y++) {
    int32_t w0 = row[0];
    row[0] += dy[0];

    int32_t w1 = row[1];
    row[1] += dy[1];

    int32_t w2 = row[2];
    row[2] += dy[2];

    for (point.x = min.x; point.x <= max.x; point.x++) {
      if (w0 > c[0] && w1 > c[1] && w2 > c[2]) {
        auto color = color_lerp(triangle.colors, w0, w1, w2);
        auto coord = point_lerp(triangle.coords, w0, w1, w2);

        draw_color(command, color, point, coord, triangle.tev);
      }

      w0 += dx[0];
      w1 += dx[1];
      w2 += dx[2];
    }
  }
}

static void put_in_clockwise_order(
  point *points, color *colors,
  texture_coord *coords, triangle *triangle) {
  int32_t indices[3];

  if (is_clockwise(points)) {
    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;
  } else {
    indices[0] = 0;
    indices[1] = 2;
    indices[2] = 1;
  }

  triangle->colors[0] = colors[indices[0]];
  triangle->colors[1] = colors[indices[1]];
  triangle->colors[2] = colors[indices[2]];

  triangle->coords[0] = coords[indices[0]];
  triangle->coords[1] = coords[indices[1]];
  triangle->coords[2] = coords[indices[2]];

  triangle->points[0] = points[indices[0]];
  triangle->points[1] = points[indices[1]];
  triangle->points[2] = points[indices[2]];
}

void core::draw_polygon() {
  color colors[4];
  texture_coord coords[4];
  point points[4];

  auto command = gp0_command(fifo.at(0));

  auto num_vertices = command.is_quad_poly() ? 4 : 3;
  auto num_polygons = command.is_quad_poly() ? 2 : 1;

  get_colors(*this, command, colors, num_vertices);
  get_coords(*this, command, coords, num_vertices);
  get_points(*this, command, points, num_vertices);

  triangle triangle;
  triangle.tev = get_tev(*this, command);

  for (int32_t i = 0; i < num_polygons; i++) {
    put_in_clockwise_order(
      &points[i],
      &colors[i],
      &coords[i], &triangle);

    draw_triangle(command, triangle);
  }
}
