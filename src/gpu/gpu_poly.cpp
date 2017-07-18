#include "gpu_core.hpp"
#include "../memory/vram.hpp"

struct triangle_t {
  gpu::color_t *colors;
  gpu::point_t *coords;
  gpu::point_t *points;

  gpu::tev_t *tev;
};

static int get_color_factor(uint32_t command) {
  static const int lut[4] = { 0, 0, 2, 3 };

  int bit1 = (command >> 28) & 1;
  int bit0 = (command >> 26) & 1;

  return lut[(bit1 << 1) | bit0];
}

static int get_point_factor(uint32_t command) {
  static const int lut[4] = { 1, 2, 2, 3 };

  int bit1 = (command >> 28) & 1;
  int bit0 = (command >> 26) & 1;

  return lut[(bit1 << 1) | bit0];
}

static int get_coord_factor(uint32_t command) {
  static const int lut[4] = { 0, 2, 0, 3 };

  int bit1 = (command >> 28) & 1;
  int bit0 = (command >> 26) & 1;

  return lut[(bit1 << 1) | bit0];
}

static gpu::color_t decode_color(gpu_state_t *state, int n) {
  uint32_t value = state->fifo.buffer[n];

  gpu::color_t result;

  result.r = utility::uclip<8>(value >> (8 * 0));
  result.g = utility::uclip<8>(value >> (8 * 1));
  result.b = utility::uclip<8>(value >> (8 * 2));

  return result;
}

static gpu::point_t decode_point(gpu_state_t *state, int n) {
  uint32_t value = state->fifo.buffer[n];

  gpu::point_t result;

  result.x = state->x_offset + utility::sclip<11>(value);
  result.y = state->y_offset + utility::sclip<11>(value >> 16);

  return result;
}

static gpu::point_t decode_coord(gpu_state_t *state, int n) {
  uint32_t value = state->fifo.buffer[n];

  gpu::point_t result;

  result.x = utility::uclip<8>(value >> 0);
  result.y = utility::uclip<8>(value >> 8);

  return result;
}

static void get_colors(gpu_state_t *state, uint32_t command, gpu::color_t *colors, int n) {
  int factor = get_color_factor(command);

  for (int i = 0; i < n; i++) {
    colors[i] = decode_color(state, i * factor + 0);
  }
}

static void get_points(gpu_state_t *state, uint32_t command, gpu::point_t *points, int n) {
  int factor = get_point_factor(command);

  for (int i = 0; i < n; i++) {
    points[i] = decode_point(state, i * factor + 1);
  }
}

static void get_coords(gpu_state_t *state, uint32_t command, gpu::point_t *coords, int n) {
  int factor = get_coord_factor(command);

  for (int i = 0; i < n; i++) {
    coords[i] = decode_coord(state, i * factor + 2);
  }
}

static gpu::tev_t get_tev(gpu_state_t *state, uint32_t command) {
  int factor = get_coord_factor(command);

  uint32_t palette = state->fifo.buffer[0 * factor + 2] >> 16;
  uint32_t texpage = state->fifo.buffer[1 * factor + 2] >> 16;

  gpu::tev_t result;

  //  11    Texture Disable (0=Normal, 1=Disable if GP1(09h).Bit0=1)   ;GPUSTAT.15

  result.color_mix_mode = (texpage >> 5) & 3;
  result.palette_page_x = (palette << 4) & 0x3f0;
  result.palette_page_y = (palette >> 6) & 0x1ff;
  result.texture_colors = (texpage >> 7) & 3;
  result.texture_page_x = (texpage << 6) & 0x3c0;
  result.texture_page_y = (texpage << 4) & 0x100;

  return result;
}

static bool is_clockwise(const gpu::point_t *p) {
  int sum =
      (p[1].x - p[0].x) * (p[1].y + p[0].y) +
      (p[2].x - p[1].x) * (p[2].y + p[1].y) +
      (p[0].x - p[2].x) * (p[0].y + p[2].y);

  return sum >= 0;
}

static int edge_function(const gpu::point_t &a, const gpu::point_t &b, const gpu::point_t &c) {
  return
    (a.x - b.x) * (c.y - b.y) -
    (a.y - b.y) * (c.x - b.x);
}

static gpu::color_t color_lerp(const gpu::color_t &c0, int w0, const gpu::color_t &c1, int w1, const gpu::color_t &c2, int w2) {
  gpu::color_t color;
  color.r = ((w0 * c0.r) + (w1 * c1.r) + (w2 * c2.r)) / (w0 + w1 + w2);
  color.g = ((w0 * c0.g) + (w1 * c1.g) + (w2 * c2.g)) / (w0 + w1 + w2);
  color.b = ((w0 * c0.b) + (w1 * c1.b) + (w2 * c2.b)) / (w0 + w1 + w2);

  return color;
}

static gpu::point_t point_lerp(const gpu::point_t &t0, int w0, const gpu::point_t &t1, int w1, const gpu::point_t &t2, int w2) {
  gpu::point_t point;
  point.x = ((w0 * t0.x) + (w1 * t1.x) + (w2 * t2.x)) / (w0 + w1 + w2);
  point.y = ((w0 * t0.y) + (w1 * t1.y) + (w2 * t2.y)) / (w0 + w1 + w2);

  return point;
}

static gpu::color_t get_color__4bpp(gpu::tev_t &tev, gpu::point_t &coord) {
  uint16_t texel = vram::read(tev.texture_page_x + coord.x / 4,
                              tev.texture_page_y + coord.y);

  texel = (texel >> ((coord.x & 3) * 4)) & 0x0f;

  uint16_t pixel = vram::read(tev.palette_page_x + texel,
                              tev.palette_page_y);

  gpu::color_t color;
  color.r = (pixel << 3) & 0xf8;
  color.g = (pixel >> 2) & 0xf8;
  color.b = (pixel >> 7) & 0xf8;

  return color;
}

static gpu::color_t get_color__8bpp(gpu::tev_t &tev, gpu::point_t &coord) {
  uint16_t texel = vram::read(tev.texture_page_x + coord.x / 2,
                              tev.texture_page_y + coord.y);

  texel = (texel >> ((coord.x & 1) * 8)) & 0xff;

  uint16_t pixel = vram::read(tev.palette_page_x + texel,
                              tev.palette_page_y);

  gpu::color_t color;
  color.r = (pixel << 3) & 0xf8;
  color.g = (pixel >> 2) & 0xf8;
  color.b = (pixel >> 7) & 0xf8;

  return color;
}

static gpu::color_t get_color_15bpp(gpu::tev_t &tev, gpu::point_t &coord) {
  uint16_t pixel = vram::read(tev.texture_page_x + coord.x,
                              tev.texture_page_y + coord.y);

  gpu::color_t color;
  color.r = (pixel << 3) & 0xf8;
  color.g = (pixel >> 2) & 0xf8;
  color.b = (pixel >> 7) & 0xf8;

  return color;
}

static void draw_triangle(gpu_state_t *state, uint32_t command, triangle_t &triangle) {
  int indices[3];

  if (is_clockwise(triangle.points)) {
    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;
  }
  else {
    indices[0] = 0;
    indices[1] = 2;
    indices[2] = 1;
  }

  const gpu::color_t &c0 = triangle.colors[indices[0]];
  const gpu::color_t &c1 = triangle.colors[indices[1]];
  const gpu::color_t &c2 = triangle.colors[indices[2]];

  const gpu::point_t &t0 = triangle.coords[indices[0]];
  const gpu::point_t &t1 = triangle.coords[indices[1]];
  const gpu::point_t &t2 = triangle.coords[indices[2]];

  const gpu::point_t &v0 = triangle.points[indices[0]];
  const gpu::point_t &v1 = triangle.points[indices[1]];
  const gpu::point_t &v2 = triangle.points[indices[2]];

  gpu::point_t min;
  min.x = std::min(v0.x, std::min(v1.x, v2.x));
  min.y = std::min(v0.y, std::min(v1.y, v2.y));

  gpu::point_t max;
  max.x = std::max(v0.x, std::max(v1.x, v2.x));
  max.y = std::max(v0.y, std::max(v1.y, v2.y));

  int a01 = v1.y - v0.y, b01 = v0.x - v1.x;
  int a12 = v2.y - v1.y, b12 = v1.x - v2.x;
  int a20 = v0.y - v2.y, b20 = v2.x - v0.x;

  int w0_row = edge_function(min, v1, v2);
  int w1_row = edge_function(min, v2, v0);
  int w2_row = edge_function(min, v0, v1);

  //  0-23  Color for (first) Vertex                   (Not for Raw-Texture)
  //  24    Texture Mode      (0=Blended, 1=Raw)       (Textured-Polygon/Rect only)
  //  25    Semi Transparency (0=Off, 1=On)            (All Render Types)
  //  26    Texture Mapping   (0=Off, 1=On)            (Polygon/Rectangle only)
  //  28    Shading           (0=Flat, 1=Gouroud)      (Polygon/Line only)

  gpu::point_t point;

  for (point.y = min.y; point.y < max.y; point.y++) {
    int w0 = w0_row;
    int w1 = w1_row;
    int w2 = w2_row;

    for (point.x = min.x; point.x < max.x; point.x++) {
      if (w0 > 0 && w1 > 0 && w2 > 0) {
        if (command & (1 << 26)) {
          gpu::color_t color;
          gpu::point_t coord = point_lerp(t0, w0, t1, w1, t2, w2);

          switch (triangle.tev->texture_colors) {
          case 0: color = get_color__4bpp(*triangle.tev, coord); break;
          case 1: color = get_color__8bpp(*triangle.tev, coord); break;
          case 2: color = get_color_15bpp(*triangle.tev, coord); break;
          case 3: color = get_color_15bpp(*triangle.tev, coord); break;
          }

          if (color.r | color.g | color.b) {
            gpu::draw_point(state, point, color);
          }
        }
        else {
          gpu::color_t color = color_lerp(c0, w0, c1, w1, c2, w2);

          gpu::draw_point(state, point, color);
        }
      }

      w0 += a12;
      w1 += a20;
      w2 += a01;
    }

    w0_row += b12;
    w1_row += b20;
    w2_row += b01;
  }
}

void gpu::draw_polygon(gpu_state_t *state) {
  gpu::color_t colors[4];
  gpu::point_t coords[4];
  gpu::point_t points[4];

  uint32_t command = state->fifo.buffer[0];

  int num_polygons = (command & (1 << 27)) ? 2 : 1;
  int num_vertices = (command & (1 << 27)) ? 4 : 3;

  get_colors(state, command, colors, num_vertices);
  get_coords(state, command, coords, num_vertices);
  get_points(state, command, points, num_vertices);

  auto tev = get_tev(state, command);

  for (int i = 0; i < num_polygons; i++) {
    triangle_t triangle;
    triangle.colors = &colors[i];
    triangle.coords = &coords[i];
    triangle.points = &points[i];
    triangle.tev = &tev;

    draw_triangle(state, command, triangle);
  }
}
