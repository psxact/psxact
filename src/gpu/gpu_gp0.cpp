#include <cassert>
#include "gpu_core.hpp"
#include "../memory/vram.hpp"
#include "../state.hpp"

static int command_size[256] = {
  1, 1, 3, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1, // $00
  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1, // $10
  4, 1, 1, 1,  1, 1, 1, 1,  5, 1, 5, 1,  9, 9, 9, 1, // $20
  6, 1, 1, 1,  9, 1, 1, 1,  8, 1, 1, 1, 12, 1,12, 1, // $30

  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1, // $40
  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1, // $50
  1, 1, 1, 1,  4, 4, 4, 4,  2, 1, 1, 1,  3, 3, 3, 3, // $60
  1, 1, 1, 1,  3, 3, 3, 3,  1, 1, 1, 1,  3, 3, 3, 3, // $70

  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1, // $80
  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1, // $90
  3, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1, // $a0
  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1, // $b0

  3, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1, // $c0
  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1, // $d0
  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1, // $e0
  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1, // $f0
};

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

    switch (command & 0xe0) {
    case 0x20: return draw_polygon(state);
    case 0x40: return draw_line(state);
    case 0x60: return draw_rectangle(state);
    }

    switch (command) {
    case 0x00: break; // nop
    case 0x01: break; // clear texture cache

    case 0x02: {
      uint16_t color =
        ((state->fifo.buffer[0] >> 3) & 0x001f) |
        ((state->fifo.buffer[0] >> 6) & 0x03e0) |
        ((state->fifo.buffer[0] >> 9) & 0x7c00);

      gpu::point_t point;
      point.x = (state->fifo.buffer[1] + 0x0) & 0x3f0;
      point.y = (state->fifo.buffer[1] >> 16) & 0x1ff;

      gpu::point_t count;
      count.x = (state->fifo.buffer[2] + 0xf) & 0x7f0;
      count.y = (state->fifo.buffer[2] >> 16) & 0x1ff;

      for (int y = 0; y < count.y; y++) {
        for (int x = 0; x < count.x; x++) {
          vram::write(point.x + x,
                      point.y + y,
                      color);
        }
      }
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
      state->drawing_area_x1 = (state->fifo.buffer[0] >>  0) & 0x3ff;
      state->drawing_area_y1 = (state->fifo.buffer[0] >> 10) & 0x3ff;
      break;

    case 0xe4:
      state->drawing_area_x2 = (state->fifo.buffer[0] >>  0) & 0x3ff;
      state->drawing_area_y2 = (state->fifo.buffer[0] >> 10) & 0x3ff;
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
      if (command_size[command] == 1) {
        printf("unhandled gp0 command: 0x%08x\n", state->fifo.buffer[0]);
      }
      break;
    }
  }
}
