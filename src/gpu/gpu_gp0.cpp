#include <cassert>
#include "gpu_core.hpp"
#include "../memory/vram.hpp"

static int command_size[256] = {
  1, 1, 3, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1, // $00
  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1, // $10
  4, 1, 1, 1,  1, 1, 1, 1,  5, 1, 1, 1,  9, 9, 1, 1, // $20
  6, 1, 1, 1,  1, 1, 1, 1,  8, 1, 1, 1,  1, 1, 1, 1, // $30

  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1, // $40
  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1, // $50
  1, 1, 1, 1,  1, 4, 1, 1,  2, 1, 1, 1,  1, 1, 1, 1, // $60
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

static gpu::point_t gp0_to_point(uint32_t point) {
  gpu::point_t result;
  result.x = utility::sclip<11>(point);
  result.y = utility::sclip<11>(point >> 16);

  return result;
}

static gpu::color_t gp0_to_color(uint32_t color) {
  gpu::color_t result;
  result.r = (color >> 0) & 0xff;
  result.g = (color >> 8) & 0xff;
  result.b = (color >> 16) & 0xff;

  return result;
}

static gpu::gouraud::pixel_t gp0_to_gouraud_pixel(uint32_t point, uint32_t color) {
  gpu::gouraud::pixel_t p;
  p.point = gp0_to_point(point);
  p.color = gp0_to_color(color);

  return p;
}

static gpu::texture::pixel_t gp0_to_texture_pixel(uint32_t point, uint32_t color, uint32_t coord) {
  gpu::texture::pixel_t p;
  p.point = gp0_to_point(point);
  p.color = gp0_to_color(color);

  p.u = (coord >> 0) & 0xff;
  p.v = (coord >> 8) & 0xff;

  return p;
}

void gpu::gp0(gpu_state_t *state, uint32_t data) {
  if (state->cpu_to_gpu_transfer.run.active) {
    auto lower = uint16_t(data >>  0);
    auto upper = uint16_t(data >> 16);

    vram_transfer(state, lower);
    vram_transfer(state, upper);
    return;
  }

  state->fifo.buffer[state->fifo.wr] = data;
  state->fifo.wr = (state->fifo.wr + 1) & 0xf;

  auto command = state->fifo.buffer[0] >> 24;

  if (state->fifo.wr == command_size[command]) {
    state->fifo.wr = 0;

    switch (command) {
    case 0x00: break; // nop
    case 0x01: break; // clear texture cache

    case 0x02: { // fill rectangle
      auto color = gp0_to_color(state->fifo.buffer[0]);
      auto point1 = gp0_to_point(state->fifo.buffer[1]);
      auto point2 = gp0_to_point(state->fifo.buffer[2]);

      point1.x = (point1.x + 0x0) & ~0xf;
      point2.x = (point2.x + 0xf) & ~0xf;

      for (int y = 0; y < point2.y; y++) {
        for (int x = 0; x < point2.x; x++) {
          gpu::draw_point(
            state,
            point1.x + x,
            point1.y + y,
            color.r,
            color.g,
            color.b);
        }
      }

      break;
    }

    case 0x20: { // monochrome triangle, opaque
      auto color = state->fifo.buffer[0];
      auto point1 = state->fifo.buffer[1];
      auto point2 = state->fifo.buffer[2];
      auto point3 = state->fifo.buffer[3];

      auto v0 = gp0_to_gouraud_pixel(point1, color);
      auto v1 = gp0_to_gouraud_pixel(point2, color);
      auto v2 = gp0_to_gouraud_pixel(point3, color);

      gpu::gouraud::draw_poly3(state, { v0, v1, v2 });
      break;
    }

    case 0x28: { // monochrome quad, opaque
      auto color = state->fifo.buffer[0];
      auto point1 = state->fifo.buffer[1];
      auto point2 = state->fifo.buffer[2];
      auto point3 = state->fifo.buffer[3];
      auto point4 = state->fifo.buffer[4];

      auto v0 = gp0_to_gouraud_pixel(point1, color);
      auto v1 = gp0_to_gouraud_pixel(point2, color);
      auto v2 = gp0_to_gouraud_pixel(point3, color);
      auto v3 = gp0_to_gouraud_pixel(point4, color);

      gpu::gouraud::draw_poly4(state, { v0, v1, v2, v3 });
      break;
    }

    case 0x2c:
    case 0x2d: { // textured quad, opaque
      auto color = state->fifo.buffer[0];
      auto point1 = state->fifo.buffer[1];
      auto coord1 = state->fifo.buffer[2];
      auto point2 = state->fifo.buffer[3];
      auto coord2 = state->fifo.buffer[4];
      auto point3 = state->fifo.buffer[5];
      auto coord3 = state->fifo.buffer[6];
      auto point4 = state->fifo.buffer[7];
      auto coord4 = state->fifo.buffer[8];

      gpu::texture::polygon_t<4> p;

      p.v[0] = gp0_to_texture_pixel(point1, color, coord1);
      p.v[1] = gp0_to_texture_pixel(point2, color, coord2);
      p.v[2] = gp0_to_texture_pixel(point3, color, coord3);
      p.v[3] = gp0_to_texture_pixel(point4, color, coord4);
      p.clut_x = ((coord1 >> 16) & 0x03f) * 16;
      p.clut_y = ((coord1 >> 22) & 0x1ff) * 1;
      p.base_u = ((coord2 >> 16) & 0x00f) * 64;
      p.base_v = ((coord2 >> 20) & 0x001) * 256;
      p.depth = ((coord2 >> 23) & 0x003);

      gpu::texture::draw_poly4(state, p);
      break;
    }

    case 0x30: { // shaded triangle, opaque
      auto color1 = state->fifo.buffer[0];
      auto point1 = state->fifo.buffer[1];
      auto color2 = state->fifo.buffer[2];
      auto point2 = state->fifo.buffer[3];
      auto color3 = state->fifo.buffer[4];
      auto point3 = state->fifo.buffer[5];

      auto v0 = gp0_to_gouraud_pixel(point1, color1);
      auto v1 = gp0_to_gouraud_pixel(point2, color2);
      auto v2 = gp0_to_gouraud_pixel(point3, color3);

      gpu::gouraud::draw_poly3(state, { v0, v1, v2 });
      break;
    }

    case 0x38: { // shaded quad, opaque
      auto color1 = state->fifo.buffer[0];
      auto point1 = state->fifo.buffer[1];
      auto color2 = state->fifo.buffer[2];
      auto point2 = state->fifo.buffer[3];
      auto color3 = state->fifo.buffer[4];
      auto point3 = state->fifo.buffer[5];
      auto color4 = state->fifo.buffer[6];
      auto point4 = state->fifo.buffer[7];

      auto v0 = gp0_to_gouraud_pixel(point1, color1);
      auto v1 = gp0_to_gouraud_pixel(point2, color2);
      auto v2 = gp0_to_gouraud_pixel(point3, color3);
      auto v3 = gp0_to_gouraud_pixel(point4, color4);

      gpu::gouraud::draw_poly4(state, { v0, v1, v2, v3 });
      break;
    }

    case 0x65: {
      //auto color  = gp0_to_color(state.fifo.buffer[0]);
      auto point1 = gp0_to_point(state->fifo.buffer[1]);
      auto coord = state->fifo.buffer[2];
      auto point2 = gp0_to_point(state->fifo.buffer[3]);

      assert((state->status & 0x180) == 0);

      auto base_u = ((state->status >> 0) & 0xf) * 64;
      auto base_v = ((state->status >> 4) & 0x1) * 256;

      auto clut_x = ((coord >> 16) & 0x03f) * 16;
      auto clut_y = ((coord >> 22) & 0x1ff);

      for (int y = 0; y < point2.y; y++) {
        for (int x = 0; x < point2.x; x++) {
          auto texel = vram::read(base_u + (x / 4),
            base_v + y);

          int index = 0;

          switch (x & 3) {
          case 0: index = (texel >> 0) & 0xf; break;
          case 1: index = (texel >> 4) & 0xf; break;
          case 2: index = (texel >> 8) & 0xf; break;
          case 3: index = (texel >> 12) & 0xf; break;
          }

          auto color = vram::read(clut_x + index,
            clut_y);

          vram::write(point1.x + x,
            point1.y + y,
            color);
        }
      }

      break;
    }

    case 0x68: {
      auto color = gp0_to_color(state->fifo.buffer[0]);
      auto point = gp0_to_point(state->fifo.buffer[1]);

      gpu::draw_point(
        state,
        point.x,
        point.y,
        color.r,
        color.g,
        color.b);
      break;
    }

    case 0xa0: {
      auto &transfer = state->cpu_to_gpu_transfer;
      transfer.reg.x = state->fifo.buffer[1] & 0xffff;
      transfer.reg.y = state->fifo.buffer[1] >> 16;
      transfer.reg.w = state->fifo.buffer[2] & 0xffff;
      transfer.reg.h = state->fifo.buffer[2] >> 16;

      transfer.run.x = 0;
      transfer.run.y = 0;
      transfer.run.active = true;
      break;
    }

    case 0xc0: {
      auto &transfer = state->gpu_to_cpu_transfer;
      transfer.reg.x = state->fifo.buffer[1] & 0xffff;
      transfer.reg.y = state->fifo.buffer[1] >> 16;
      transfer.reg.w = state->fifo.buffer[2] & 0xffff;
      transfer.reg.h = state->fifo.buffer[2] >> 16;

      transfer.run.x = 0;
      transfer.run.y = 0;
      transfer.run.active = true;
      break;
    }

    case 0xe1:
      state->status &= ~0x87ff;
      state->status |= (state->fifo.buffer[0] << 0) & 0x7ff;
      state->status |= (state->fifo.buffer[0] << 4) & 0x8000;

      state->textured_rectangle_x_flip = ((state->fifo.buffer[0] >> 12) & 1) != 0;
      state->textured_rectangle_y_flip = ((state->fifo.buffer[0] >> 13) & 1) != 0;
      break;

    case 0xe2:
      state->texture_window_mask_x = utility::uclip<5>(state->fifo.buffer[0] >> 0);
      state->texture_window_mask_y = utility::uclip<5>(state->fifo.buffer[0] >> 5);
      state->texture_window_offset_x = utility::uclip<5>(state->fifo.buffer[0] >> 10);
      state->texture_window_offset_y = utility::uclip<5>(state->fifo.buffer[0] >> 15);
      break;

    case 0xe3:
      state->drawing_area_x1 = utility::uclip<10>(state->fifo.buffer[0] >> 0);
      state->drawing_area_y1 = utility::uclip<10>(state->fifo.buffer[0] >> 10);
      break;

    case 0xe4:
      state->drawing_area_x2 = utility::uclip<10>(state->fifo.buffer[0] >> 0);
      state->drawing_area_y2 = utility::uclip<10>(state->fifo.buffer[0] >> 10);
      break;

    case 0xe5:
      state->x_offset = utility::sclip<11>(state->fifo.buffer[0] >> 0);
      state->y_offset = utility::sclip<11>(state->fifo.buffer[0] >> 11);
      break;

    case 0xe6:
      state->status &= ~0x1800;
      state->status |= (state->fifo.buffer[0] << 11) & 0x1800;
      break;

    default:
      printf("unhandled gp0 command: 0x%08x\n", state->fifo.buffer[0]);
      break;
    }
  }
}
