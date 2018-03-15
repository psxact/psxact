#include <algorithm>
#include "gpu/gpu.hpp"
#include "memory/vram.hpp"
#include "utility.hpp"


struct triangle_t {
  gpu_t::color_t colors[3];
  gpu_t::point_t coords[3];
  gpu_t::point_t points[3];

  gpu_t::tev_t tev;
};


static const int32_t point_factor_lut[4] = {
    1, 2, 2, 3
};


static const int32_t color_factor_lut[4] = {
    0, 0, 2, 3
};


static const int32_t coord_factor_lut[4] = {
    0, 2, 0, 3
};


static int32_t get_factor(const int32_t (&lut)[4], uint32_t command) {
  int32_t bit1 = (command >> 28) & 1;
  int32_t bit0 = (command >> 26) & 1;

  return lut[(bit1 << 1) | bit0];
}


static int32_t get_color_factor(uint32_t command) {
  return get_factor(color_factor_lut, command);
}


static int32_t get_point_factor(uint32_t command) {
  return get_factor(point_factor_lut, command);
}


static int32_t get_coord_factor(uint32_t command) {
  return get_factor(coord_factor_lut, command);
}


static gpu_t::color_t decode_color(gpu_t &state, int32_t n) {
  uint32_t value = state.fifo.buffer[n];

  gpu_t::color_t result;

  result.r = utility::uclip<8>(value >> (8 * 0));
  result.g = utility::uclip<8>(value >> (8 * 1));
  result.b = utility::uclip<8>(value >> (8 * 2));

  return result;
}


static gpu_t::point_t decode_point(gpu_t &state, int32_t n) {
  uint32_t value = state.fifo.buffer[n];

  gpu_t::point_t result;

  result.x = state.x_offset + utility::sclip<11>(value);
  result.y = state.y_offset + utility::sclip<11>(value >> 16);

  return result;
}


static gpu_t::point_t decode_coord(gpu_t &state, int32_t n) {
  uint32_t value = state.fifo.buffer[n];

  gpu_t::point_t result;

  result.x = utility::uclip<8>(value >> 0);
  result.y = utility::uclip<8>(value >> 8);

  return result;
}


static void get_colors(gpu_t &state, uint32_t command, gpu_t::color_t *colors, int32_t n) {
  int32_t factor = get_color_factor(command);

  for (int32_t i = 0; i < n; i++) {
    colors[i] = decode_color(state, i * factor + 0);
  }
}


static void get_points(gpu_t &state, uint32_t command, gpu_t::point_t *points, int32_t n) {
  int32_t factor = get_point_factor(command);

  for (int32_t i = 0; i < n; i++) {
    points[i] = decode_point(state, i * factor + 1);
  }
}


static void get_coords(gpu_t &state, uint32_t command, gpu_t::point_t *coords, int32_t n) {
  int32_t factor = get_coord_factor(command);

  for (int32_t i = 0; i < n; i++) {
    coords[i] = decode_coord(state, i * factor + 2);
  }
}


static gpu_t::tev_t get_tev(gpu_t &state, uint32_t command) {
  int32_t factor = get_coord_factor(command);

  uint32_t palette = state.fifo.buffer[0 * factor + 2] >> 16;
  uint32_t texpage = state.fifo.buffer[1 * factor + 2] >> 16;

  gpu_t::tev_t result;

  //  11    Texture Disable (0=Normal, 1=Disable if GP1(09h).Bit0=1)   ;GPUSTAT.15

  result.palette_page_x = (palette << 4) & 0x3f0;
  result.palette_page_y = (palette >> 6) & 0x1ff;
  result.texture_colors = (texpage >> 7) & 3;
  result.texture_page_x = (texpage << 6) & 0x3c0;
  result.texture_page_y = (texpage << 4) & 0x100;

  if (command & (1 << 26)) {
    result.color_mix_mode = (texpage >> 5) & 3;
  }
  else {
    result.color_mix_mode = (state.status >> 5) & 3;
  }

  return result;
}


static bool is_clockwise(const gpu_t::point_t *p) {
  int32_t sum =
      (p[1].x - p[0].x) * (p[1].y + p[0].y) +
      (p[2].x - p[1].x) * (p[2].y + p[1].y) +
      (p[0].x - p[2].x) * (p[0].y + p[2].y);

  return sum >= 0;
}


static int32_t edge_function(const gpu_t::point_t &a, const gpu_t::point_t &b, const gpu_t::point_t &c) {
  return
      (a.x - b.x) * (c.y - b.y) -
      (a.y - b.y) * (c.x - b.x);
}


static gpu_t::color_t color_lerp(const gpu_t::color_t *c, int32_t w0, int32_t w1, int32_t w2) {
  gpu_t::color_t color;
  color.r = ((w0 * c[0].r) + (w1 * c[1].r) + (w2 * c[2].r)) / (w0 + w1 + w2);
  color.g = ((w0 * c[0].g) + (w1 * c[1].g) + (w2 * c[2].g)) / (w0 + w1 + w2);
  color.b = ((w0 * c[0].b) + (w1 * c[1].b) + (w2 * c[2].b)) / (w0 + w1 + w2);

  return color;
}


static gpu_t::point_t point_lerp(const gpu_t::point_t *t, int32_t w0, int32_t w1, int32_t w2) {
  gpu_t::point_t point;
  point.x = ((w0 * t[0].x) + (w1 * t[1].x) + (w2 * t[2].x)) / (w0 + w1 + w2);
  point.y = ((w0 * t[0].y) + (w1 * t[1].y) + (w2 * t[2].y)) / (w0 + w1 + w2);

  return point;
}


static bool get_color(uint32_t command, triangle_t &triangle, int32_t w0, int32_t w1, int32_t w2, gpu_t::color_t &color) {
  bool shaded = (command & (1 << 26)) == 0;
  bool blended = (command & (1 << 24)) != 0;

  if (shaded) {
    color = color_lerp(triangle.colors, w0, w1, w2);
    return true;
  }

  gpu_t::point_t coord = point_lerp(triangle.coords, w0, w1, w2);

  if (blended) {
    color = gpu_t::get_texture_color(triangle.tev, coord);
  }
  else {
    gpu_t::color_t color1 = gpu_t::get_texture_color(triangle.tev, coord);
    gpu_t::color_t color2 = color_lerp(triangle.colors, w0, w1, w2);

    color.r = std::min(255, (color1.r * color2.r) / 128);
    color.g = std::min(255, (color1.g * color2.g) / 128);
    color.b = std::min(255, (color1.b * color2.b) / 128);
  }

  return (color.r | color.g | color.b) > 0;
}


static void draw_triangle(gpu_t &state, uint32_t command, triangle_t &triangle) {
  const gpu_t::point_t *v = triangle.points;

  gpu_t::point_t min;
  min.x = std::min(v[0].x, std::min(v[1].x, v[2].x));
  min.y = std::min(v[0].y, std::min(v[1].y, v[2].y));

  gpu_t::point_t max;
  max.x = std::max(v[0].x, std::max(v[1].x, v[2].x));
  max.y = std::max(v[0].y, std::max(v[1].y, v[2].y));

  min.x = std::max(min.x, state.drawing_area_x1);
  min.y = std::max(min.y, state.drawing_area_y1);
  max.x = std::min(max.x, state.drawing_area_x2);
  max.y = std::min(max.y, state.drawing_area_y2);

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

  gpu_t::point_t point;

  for (point.y = min.y; point.y <= max.y; point.y++) {
    int32_t w0 = row[0];
    row[0] += dy[0];

    int32_t w1 = row[1];
    row[1] += dy[1];

    int32_t w2 = row[2];
    row[2] += dy[2];

    for (point.x = min.x; point.x <= max.x; point.x++) {
      if (w0 > c[0] && w1 > c[1] && w2 > c[2]) {
        gpu_t::color_t color;

        if (get_color(command, triangle, w0, w1, w2, color)) {
          if (command & (1 << 25)) {
            gpu_t::color_t bg = gpu_t::uint16_to_color(vram::read(point.x, point.y));

            switch (triangle.tev.color_mix_mode) {
            case 0:
              color.r = (bg.r + color.r) / 2;
              color.g = (bg.g + color.g) / 2;
              color.b = (bg.b + color.b) / 2;
              break;

            case 1:
              color.r = std::min(255, bg.r + color.r);
              color.g = std::min(255, bg.g + color.g);
              color.b = std::min(255, bg.b + color.b);
              break;

            case 2:
              color.r = std::max(0, bg.r - color.r);
              color.g = std::max(0, bg.g - color.g);
              color.b = std::max(0, bg.b - color.b);
              break;

            case 3:
              color.r = std::min(255, bg.r + color.r / 4);
              color.g = std::min(255, bg.g + color.g / 4);
              color.b = std::min(255, bg.b + color.b / 4);
              break;
            }
          }

          state.draw_point(point, color);
        }
      }

      w0 += dx[0];
      w1 += dx[1];
      w2 += dx[2];
    }
  }
}


static void put_in_clockwise_order(gpu_t::point_t *points, gpu_t::color_t *colors, gpu_t::point_t *coords, triangle_t *triangle) {
  int32_t indices[3];

  if (is_clockwise(points)) {
    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;
  }
  else {
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


void gpu_t::draw_polygon() {
  color_t colors[4];
  point_t coords[4];
  point_t points[4];

  uint32_t command = fifo.buffer[0];

  int32_t num_polygons = (command & (1 << 27)) ? 2 : 1;
  int32_t num_vertices = (command & (1 << 27)) ? 4 : 3;

  get_colors(*this, command, colors, num_vertices);
  get_coords(*this, command, coords, num_vertices);
  get_points(*this, command, points, num_vertices);

  triangle_t triangle;
  triangle.tev = get_tev(*this, command);

  for (int32_t i = 0; i < num_polygons; i++) {
    put_in_clockwise_order(&points[i],
                           &colors[i],
                           &coords[i], &triangle);

    draw_triangle(*this, command, triangle);
  }
}
