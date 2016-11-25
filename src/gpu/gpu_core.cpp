#include "gpu_core.hpp"
#include "../bus.hpp"
#include <stdexcept>
#include <cassert>

static gpu::state_t state;

static int gp0_command_size[256] = {
  1, 1, 3, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1, // $00
  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1, // $10
  1, 1, 1, 1,  1, 1, 1, 1,  5, 1, 1, 1,  9, 1, 1, 1, // $20
  6, 1, 1, 1,  1, 1, 1, 1,  8, 1, 1, 1,  1, 1, 1, 1, // $30

  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1, // $40
  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1, // $50
  1, 1, 1, 1,  1, 1, 1, 1,  2, 1, 1, 1,  1, 1, 1, 1, // $60
  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1, // $70

  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1, // $80
  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1, // $90
  3, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1, // $a0
  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1, // $b0

  3, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1, // $c0
  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1, // $d0
  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1, // $e0
  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1, // $f0
};

// --=================--
//   Drawing Functions
// --=================--

gpu::vram_t gpu::vram;

uint16_t gpu::read_vram(int x, int y) {
  return vram.h[(y * 1024) + x];
}

void gpu::write_vram(int x, int y, uint16_t color) {
  vram.h[(y * 1024) + x] = color;
}

// --=============--
//   I/O Functions
// --=============--

static inline uint32_t read_texture() {
  auto &download = state.texture_download;

  if (!download.remaining) {
    return 0;
  }

  auto address = (download.current_y * 1024) + download.current_x;
  auto data = gpu::vram.h[address];

  download.remaining--;
  download.current_x++;

  if (download.current_x == (download.x + download.w)) {
    download.current_x = download.x;
    download.current_y++;
  }

  return data;
}

static inline uint32_t read_resp() {
  if (state.texture_download.remaining) {
    auto lower = read_texture();
    auto upper = read_texture();

    return (upper << 16) | lower;
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

static gpu::gouraud::pixel_t gp0_to_gouraud_pixel(uint32_t vertex, uint32_t color) {
  gpu::gouraud::pixel_t p;
  p.point.x = int(vertex & 0xffff);
  p.point.y = int(vertex >> 16);

  p.color.r = (color >>  0) & 0xff;
  p.color.g = (color >>  8) & 0xff;
  p.color.b = (color >> 16) & 0xff;

  return p;
}

static inline gpu::point_t gp0_to_point(uint32_t point) {
  gpu::point_t result;
  result.x = int(point & 0xffff);
  result.y = int(point >> 16);

  return result;
}

static gpu::color_t gp0_to_color(uint32_t color) {
  gpu::color_t result;
  result.r = (color >> 0) & 0xff;
  result.g = (color >> 8) & 0xff;
  result.b = (color >> 16) & 0xff;

  return result;
}

static gpu::texture::pixel_t gp0_to_texture_pixel(uint32_t point, uint32_t color, uint32_t texcoord) {
  gpu::texture::pixel_t p;
  p.point = gp0_to_point(point);
  p.color = gp0_to_color(color);

  p.u = (texcoord >> 0) & 0xff;
  p.v = (texcoord >> 8) & 0xff;

  return p;
}

static inline uint32_t get_gp0_data() {
  auto data = state.fifo.front();
  state.fifo.pop_front();

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

  if (state.fifo.size() == 0) {
    state.gp0_command = data >> 24;
  }

  state.fifo.push_back(data);

  if (state.fifo.size() == gp0_command_size[state.gp0_command]) {
    switch (state.gp0_command) {
      case 0x00: get_gp0_data(); break; // nop
      case 0x01: get_gp0_data(); break; // clear texture cache

      case 0x02: { // fill rectangle
        auto color  = gp0_to_color(get_gp0_data());
        auto point1 = gp0_to_point(get_gp0_data());
        auto point2 = gp0_to_point(get_gp0_data());

        point1.x = (point1.x + 0x0) & ~0xf;
        point2.x = (point2.x + 0xf) & ~0xf;

        for (int y = 0; y < point2.y; y++) {
          for (int x = 0; x < point2.x; x++) {
            gpu::draw_point(point1.x + x,
                            point1.y + y,
                            color.r,
                            color.g,
                            color.b);
          }
        }

        break;
      }

      case 0x28: { // monochrome quad, opaque
        auto color  = get_gp0_data();
        auto point1 = get_gp0_data();
        auto point2 = get_gp0_data();
        auto point3 = get_gp0_data();
        auto point4 = get_gp0_data();

        auto v0 = gp0_to_gouraud_pixel(point1, color);
        auto v1 = gp0_to_gouraud_pixel(point2, color);
        auto v2 = gp0_to_gouraud_pixel(point3, color);
        auto v3 = gp0_to_gouraud_pixel(point4, color);

        gpu::gouraud::draw_poly4({v0, v1, v2, v3});
        break;
      }

      case 0x2c: { // textured quad, opaque
        auto color  = get_gp0_data();
        auto point1 = get_gp0_data();
        auto tcoord1 = get_gp0_data();
        auto point2 = get_gp0_data();
        auto tcoord2 = get_gp0_data();
        auto point3 = get_gp0_data();
        auto tcoord3 = get_gp0_data();
        auto point4 = get_gp0_data();
        auto tcoord4 = get_gp0_data();

        gpu::texture::polygon_t<4> p;

        p.v[0] = gp0_to_texture_pixel(point1, color, tcoord1);
        p.v[1] = gp0_to_texture_pixel(point2, color, tcoord2);
        p.v[2] = gp0_to_texture_pixel(point3, color, tcoord3);
        p.v[3] = gp0_to_texture_pixel(point4, color, tcoord4);
        p.clut_x = ((tcoord1 >> 16) & 0x03f) * 16;
        p.clut_y = ((tcoord1 >> 22) & 0x1ff) * 1;
        p.base_u = ((tcoord2 >> 16) & 0x00f) * 64;
        p.base_v = ((tcoord2 >> 20) & 0x001) * 256;

        gpu::texture::draw_poly4(p);
        break;
      }

      case 0x30: { // shaded triangle, opaque
        auto color1 = get_gp0_data();
        auto point1 = get_gp0_data();
        auto color2 = get_gp0_data();
        auto point2 = get_gp0_data();
        auto color3 = get_gp0_data();
        auto point3 = get_gp0_data();

        auto v0 = gp0_to_gouraud_pixel(point1, color1);
        auto v1 = gp0_to_gouraud_pixel(point2, color2);
        auto v2 = gp0_to_gouraud_pixel(point3, color3);

        gpu::gouraud::draw_poly3({v0, v1, v2});
        break;
      }

      case 0x38: { // shaded quad, opaque
        auto color1 = get_gp0_data();
        auto point1 = get_gp0_data();
        auto color2 = get_gp0_data();
        auto point2 = get_gp0_data();
        auto color3 = get_gp0_data();
        auto point3 = get_gp0_data();
        auto color4 = get_gp0_data();
        auto point4 = get_gp0_data();

        auto v0 = gp0_to_gouraud_pixel(point1, color1);
        auto v1 = gp0_to_gouraud_pixel(point2, color2);
        auto v2 = gp0_to_gouraud_pixel(point3, color3);
        auto v3 = gp0_to_gouraud_pixel(point4, color4);

        gpu::gouraud::draw_poly4({v0, v1, v2, v3});
        break;
      }

      case 0x68: {
        auto color = gp0_to_color(get_gp0_data());
        auto point = gp0_to_point(get_gp0_data());

        gpu::draw_point(point.x,
                        point.y,
                        color.r,
                        color.g,
                        color.b);
        break;
      }

      case 0xa0: {
        auto command = get_gp0_data();
        auto param1 = get_gp0_data();
        auto param2 = get_gp0_data();

        auto &ul = state.texture_upload;
        ul.x = param1 & 0xffff;
        ul.y = param1 >> 16;
        ul.w = param2 & 0xffff;
        ul.h = param2 >> 16;

        ul.current_x = ul.x;
        ul.current_y = ul.y;
        ul.remaining = ul.w * ul.h;
        break;
      }

      case 0xc0: {
        auto command = get_gp0_data();
        auto param1 = get_gp0_data();
        auto param2 = get_gp0_data();

        auto &dl = state.texture_download;
        dl.x = param1 & 0xffff;
        dl.y = param1 >> 16;
        dl.w = param2 & 0xffff;
        dl.h = param2 >> 16;

        dl.current_x = dl.x;
        dl.current_y = dl.y;
        dl.remaining = dl.w * dl.h;
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

    case 0x01:
      state.gp0_command = 0;
      state.fifo.clear();
      state.texture_upload.remaining = 0;
      break;

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

uint32_t gpu::mmio_read(int size, uint32_t address) {
  assert(size == WORD);

  switch (address - 0x1f801810) {
    case 0: return read_resp();
    case 4: return read_stat();

    default:
      throw std::runtime_error("invalid gpu mmio_read");
  }
}

void gpu::mmio_write(int size, uint32_t address, uint32_t data) {
  assert(size == WORD);

  switch (address - 0x1f801810) {
    case 0: return write_gp0(data);
    case 4: return write_gp1(data);

    default:
      throw std::runtime_error("invalid gpu mmio_write");
  }
}
