#include "gpu_core.hpp"
#include "../utility.hpp"
#include <stdexcept>
#include <stdio.h>

static gpu::state_t state;

static int gp0_command_size[256] = {
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // $00
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // $10
  1, 1, 1, 1, 1, 1, 1, 1, 5, 1, 1, 1, 9, 1, 1, 1, // $20
  6, 1, 1, 1, 1, 1, 1, 1, 8, 1, 1, 1, 1, 1, 1, 1, // $30
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // $40
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // $50
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // $60
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // $70
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // $80
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // $90
  3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // $a0
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // $b0
  3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // $c0
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // $d0
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // $e0
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // $f0
};

static int dither_lut[4][4] = {
  { -4,  0, -3,  1 },
  {  2, -2,  3, -1 },
  { -3,  1, -4,  0 },
  {  3, -1,  2, -2 }
};

// --=================--
//   Drawing Functions
// --=================--

struct point_t {
  int x;
  int y;
  int r;
  int g;
  int b;
};

struct triangle_t {
  point_t v[3];
};

struct quad_t {
  point_t v0;
  point_t v1;
  point_t v2;
  point_t v3;
};

gpu::vram_t gpu::vram;

template<int min, int max>
int clip(int value) {
  if (value < min)
    return min;
  if (value > max)
    return max;
  return value;
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

static int get_min_point(const triangle_t &tri) {
  if (tri.v[0].y < tri.v[1].y && tri.v[0].y < tri.v[2].y) { return 0; }
  if (tri.v[1].y < tri.v[0].y && tri.v[1].y < tri.v[2].y) { return 1; }
  return 2;
}

static int get_max_point(const triangle_t &tri) {
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

void draw_upper_triangle(const point_t &v1, const point_t &v2, const point_t &v3) {
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

void draw_lower_triangle(const point_t &v1, const point_t &v2, const point_t &v3) {
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

void draw_triangle(const triangle_t &tri) {
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

  point_t v4;
  v4.x = v1.x + (((v2.y - v1.y) * (v3.x - v1.x)) / (v3.y - v1.y));
  v4.y = v2.y;
  v4.r = v3.r + (((v2.y - v1.y) * (v3.r - v1.r)) / (v3.y - v1.y));
  v4.g = v3.g + (((v2.y - v1.y) * (v3.g - v1.g)) / (v3.y - v1.y));
  v4.b = v3.b + (((v2.y - v1.y) * (v3.b - v1.b)) / (v3.y - v1.y));

  draw_upper_triangle(v1, v2, v4);
  draw_lower_triangle(v2, v4, v3);
}

void draw_quad(const quad_t &quad) {
  draw_triangle({ quad.v0, quad.v2, quad.v1 });
  draw_triangle({ quad.v1, quad.v2, quad.v3 });
}

// --=============--
//   I/O Functions
// --=============--

static inline uint32_t read_resp() {
  if (state.gp0_texture_download_size) {
    state.gp0_texture_download_size--;
    return 0;
  }

  printf("gpu::read_resp()\n");
  return 0;
}

static inline uint32_t read_stat() {
  //  19    Vertical Resolution         (0=240, 1=480, when Bit22=1)  ;GP1(08h).2
  //  26    Ready to receive Cmd Word   (0=No, 1=Ready)  ;GP0(...) ;via GP0
  //  27    Ready to send VRAM to CPU   (0=No, 1=Ready)  ;GP0(C0h) ;via GPUREAD
  //  28    Ready to receive DMA Block  (0=No, 1=Ready)  ;GP0(...) ;via GP0

  return state.status & ~(1 << 19) |
      (1 << 13) |
      (1 << 26) |
      (1 << 27) |
      (1 << 28);
}

static point_t gp0_to_point(const uint32_t &vertex, const uint32_t &color) {
  point_t p;
  p.x = int(vertex & 0xffff);
  p.y = int(vertex >> 16);
  p.r = (color >>  0) & 0xff;
  p.g = (color >>  8) & 0xff;
  p.b = (color >> 16) & 0xff;

  return p;
}

static inline uint32_t get_gp0_data() {
  auto data = state.gp0_fifo.front();
  state.gp0_fifo.pop_front();

  return data;
}

static inline void write_texture(int data) {
  auto &upload = state.texture_upload;

  if (!upload.remaining) {
    return;
  }

  auto address = (upload.current_y * 1024) + upload.current_x;
  gpu::vram.h[address] = uint16_t(data);

  upload.remaining--;
  upload.current_x++;

  if (upload.current_x == (upload.x + upload.w)) {
    upload.current_x = upload.x;
    upload.current_y++;
  }
}

static inline void write_gp0(uint32_t data) {
  if (state.texture_upload.remaining) {
    write_texture(data);
    write_texture(data >> 16);
    return;
  }

  if (state.gp0_fifo.size() == 0) {
    state.gp0_command = data >> 24;
  }

  state.gp0_fifo.push_back(data);

  if (state.gp0_fifo.size() == gp0_command_size[state.gp0_command]) {
    switch (state.gp0_command) {
      case 0x00: get_gp0_data(); break; // nop
      case 0x01: get_gp0_data(); break; // clear texture cache

      case 0x28: { // monochrome quad, opaque
        auto color   = get_gp0_data();
        auto vertex1 = get_gp0_data();
        auto vertex2 = get_gp0_data();
        auto vertex3 = get_gp0_data();
        auto vertex4 = get_gp0_data();

        auto v0 = gp0_to_point(vertex1, color);
        auto v1 = gp0_to_point(vertex2, color);
        auto v2 = gp0_to_point(vertex3, color);
        auto v3 = gp0_to_point(vertex4, color);

        draw_quad({ v0, v1, v2, v3 });
        break;
      }

      case 0x2c: { // textured quad, opaque
        auto color   = get_gp0_data();
        auto vertex1 = get_gp0_data();
        auto tcoord1 = get_gp0_data();
        auto vertex2 = get_gp0_data();
        auto tcoord2 = get_gp0_data();
        auto vertex3 = get_gp0_data();
        auto tcoord3 = get_gp0_data();
        auto vertex4 = get_gp0_data();
        auto tcoord4 = get_gp0_data();

        auto v0 = gp0_to_point(vertex1, color);
        auto v1 = gp0_to_point(vertex2, color);
        auto v2 = gp0_to_point(vertex3, color);
        auto v3 = gp0_to_point(vertex4, color);

        draw_quad({ v0, v1, v2, v3 });
        break;
      }

      case 0x30: { // shaded triangle, opaque
        auto color1  = get_gp0_data();
        auto vertex1 = get_gp0_data();
        auto color2  = get_gp0_data();
        auto vertex2 = get_gp0_data();
        auto color3  = get_gp0_data();
        auto vertex3 = get_gp0_data();

        auto v0 = gp0_to_point(vertex1, color1);
        auto v1 = gp0_to_point(vertex2, color2);
        auto v2 = gp0_to_point(vertex3, color3);

        draw_triangle({ v0, v1, v2 });
        break;
      }

      case 0x38: { // shaded quad, opaque
        auto color1  = get_gp0_data();
        auto vertex1 = get_gp0_data();
        auto color2  = get_gp0_data();
        auto vertex2 = get_gp0_data();
        auto color3  = get_gp0_data();
        auto vertex3 = get_gp0_data();
        auto color4  = get_gp0_data();
        auto vertex4 = get_gp0_data();

        auto v0 = gp0_to_point(vertex1, color1);
        auto v1 = gp0_to_point(vertex2, color2);
        auto v2 = gp0_to_point(vertex3, color3);
        auto v3 = gp0_to_point(vertex4, color4);

        draw_quad({ v0, v1, v2, v3 });
        break;
      }

      case 0xa0: {
        auto command = get_gp0_data();
        auto param1 = get_gp0_data();
        auto param2 = get_gp0_data();

        auto &upload = state.texture_upload;
        upload.x = param1 & 0xffff;
        upload.y = param1 >> 16;
        upload.w = param2 & 0xffff;
        upload.h = param2 >> 16;

        upload.current_x = upload.x;
        upload.current_y = upload.y;
        upload.remaining = upload.w * upload.h;
        break;
      }

      case 0xc0: {
        auto command = get_gp0_data();
        auto param1 = get_gp0_data();
        auto param2 = get_gp0_data();

        auto x = param1 & 0xffff;
        auto y = param1 >> 16;
        auto w = param2 & 0xffff;
        auto h = param2 >> 16;

        state.gp0_texture_download_size = ((w * h) + 1) / 2;
        break;
      }

      case 0xe1: {
        auto command = get_gp0_data();

        state.status &= ~0x87ff;
        state.status |= (command << 0) & 0x7ff;
        state.status |= (command << 4) & 0x8000;

        state.textured_rectangle_x_flip = ((command >> 12) & 1) != 0;
        state.textured_rectangle_y_flip = ((command >> 13) & 1) != 0;
        break;
      }

      case 0xe2: {
        auto command = get_gp0_data();

        state.texture_window_mask_x = utility::uclip<5>(command >> 0);
        state.texture_window_mask_y = utility::uclip<5>(command >> 5);
        state.texture_window_offset_x = utility::uclip<5>(command >> 10);
        state.texture_window_offset_y = utility::uclip<5>(command >> 15);
        break;
      }

      case 0xe3: {
        auto command = get_gp0_data();

        state.drawing_area_x1 = utility::uclip<10>(command >> 0);
        state.drawing_area_y1 = utility::uclip<10>(command >> 10);
        break;
      }

      case 0xe4: {
        auto command = get_gp0_data();

        state.drawing_area_x2 = utility::uclip<10>(command >> 0);
        state.drawing_area_y2 = utility::uclip<10>(command >> 10);
        break;
      }

      case 0xe5: {
        auto command = get_gp0_data();

        state.x_offset = utility::sclip<11>(command >> 0);
        state.y_offset = utility::sclip<11>(command >> 11);
        break;
      }

      case 0xe6: {
        auto command = get_gp0_data();

        state.status &= ~0x1800;
        state.status |= (command << 11) & 0x1800;
        break;
      }

      default: {
        auto command = get_gp0_data();

        printf("unhandled gp0 command: 0x%08x\n", command);
        break;
      }
    }
  }
}

static inline void write_gp1(uint32_t data) {
  switch ((data >> 24) & 0xff) {
    case 0x00:
      state.status = 0x14802000;
      state.textured_rectangle_x_flip = 0;
      state.textured_rectangle_y_flip = 0;
      break;

    case 0x01: {
      state.gp0_command = 0;
      state.gp0_fifo.clear();
      state.texture_upload.remaining = 0;
      break;
    }

    case 0x02:
      state.status &= ~0x01000000;
      break;

    case 0x03:
      state.status &= ~0x00800000;
      state.status |= (data << 23) & 0x00800000;
      break;

    case 0x04:
      state.status &= ~0x60000000;
      state.status |= (data << 29) & 0x60000000;
      break;

    case 0x05:
      state.display_area_x = utility::uclip<10>(data >>  0);
      state.display_area_y = utility::uclip< 9>(data >> 10);
      break;

    case 0x06:
      state.display_area_x1 = utility::uclip<12>(data >>  0);
      state.display_area_x2 = utility::uclip<12>(data >> 12);
      break;

    case 0x07:
      state.display_area_y1 = utility::uclip<10>(data >>  0);
      state.display_area_y2 = utility::uclip<10>(data >> 10);
      break;

    case 0x08:
      // 0-1   Horizontal Resolution 1     (0=256, 1=320, 2=512, 3=640) ;GPUSTAT.17-18
      // 2     Vertical Resolution         (0=240, 1=480, when Bit5=1)  ;GPUSTAT.19
      // 3     Video Mode                  (0=NTSC/60Hz, 1=PAL/50Hz)    ;GPUSTAT.20
      // 4     Display Area Color Depth    (0=15bit, 1=24bit)           ;GPUSTAT.21
      // 5     Vertical Interlace          (0=Off, 1=On)                ;GPUSTAT.22
      // 6     Horizontal Resolution 2     (0=256/320/512/640, 1=368)   ;GPUSTAT.16
      // 7     "Reverseflag"               (0=Normal, 1=Distorted)      ;GPUSTAT.14

      state.status &= ~0x7f4000;
      state.status |= (data << 17) & 0x7e0000;
      state.status |= (data << 10) & 0x10000;
      state.status |= (data <<  7) & 0x4000;
      break;

    default:
      printf("unhandled gp1 command: 0x%08x\n", data);
      break;
  }
}

uint32_t gpu::read(int size, uint32_t address) {
  switch (address) {
  case 0x1f801810: return read_resp();
  case 0x1f801814: return read_stat();
  }

  throw std::runtime_error("invalid gpu mmio_read");
}

void gpu::write(int size, uint32_t address, uint32_t data) {
  switch (address) {
  case 0x1f801810: return write_gp0(data);
  case 0x1f801814: return write_gp1(data);
  }

  throw std::runtime_error("invalid gpu mmio_write");
}
