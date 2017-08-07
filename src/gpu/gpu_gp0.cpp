#include "gpu_core.hpp"
#include "../memory/vram.hpp"
#include "../utility.hpp"

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

void gpu_core::fill_rectangle() {
  uint16_t color =
      ((fifo.buffer[0] >> 3) & 0x001f) |
      ((fifo.buffer[0] >> 6) & 0x03e0) |
      ((fifo.buffer[0] >> 9) & 0x7c00);

  gpu_core::point_t point;
  point.x = (fifo.buffer[1] + 0x0) & 0x3f0;
  point.y = (fifo.buffer[1] >> 16) & 0x1ff;

  gpu_core::point_t count;
  count.x = (fifo.buffer[2] + 0xf) & 0x7f0;
  count.y = (fifo.buffer[2] >> 16) & 0x1ff;
  count.x += point.x;
  count.y += point.y;

  for (; point.y < count.y; point.y++) {
    for (; point.x < count.x; point.x++) {
      vram::write(point.x,
                  point.y,
                  color);
    }
  }
}

void gpu_core::copy_vram_to_vram() {}

void gpu_core::copy_wram_to_vram() {
  auto &transfer = cpu_to_gpu_transfer;
  transfer.reg.x = fifo.buffer[1] & 0xffff;
  transfer.reg.y = fifo.buffer[1] >> 16;
  transfer.reg.w = fifo.buffer[2] & 0xffff;
  transfer.reg.h = fifo.buffer[2] >> 16;

  transfer.run.x = 0;
  transfer.run.y = 0;
  transfer.run.active = true;
}

void gpu_core::copy_vram_to_wram() {
  auto &transfer = gpu_to_cpu_transfer;
  transfer.reg.x = fifo.buffer[1] & 0xffff;
  transfer.reg.y = fifo.buffer[1] >> 16;
  transfer.reg.w = fifo.buffer[2] & 0xffff;
  transfer.reg.h = fifo.buffer[2] >> 16;

  transfer.run.x = 0;
  transfer.run.y = 0;
  transfer.run.active = true;
}

void gpu_core::gp0(uint32_t data) {
  if (cpu_to_gpu_transfer.run.active) {
    uint16_t lower = uint16_t(data >>  0);
    uint16_t upper = uint16_t(data >> 16);

    vram_transfer(lower);
    vram_transfer(upper);
    return;
  }

  fifo.buffer[fifo.wr] = data;
  fifo.wr = (fifo.wr + 1) & 0xf;

  uint32_t command = fifo.buffer[0] >> 24;

  if (fifo.wr == command_size[command]) {
    fifo.wr = 0;

    switch (command & 0xe0) {
    case 0x20:
      return draw_polygon();

    case 0x40:
      return draw_line();

    case 0x60:
      return draw_rectangle();

    case 0x80:
      return copy_vram_to_vram();

    case 0xa0:
      return copy_wram_to_vram();

    case 0xc0:
      return copy_vram_to_wram();
    }

    switch (command) {
    case 0x00: // nop
      break;

    case 0x01: // clear texture cache
      break;

    case 0x02:
      return fill_rectangle();

    case 0xe1:
      status &= ~0x87ff;
      status |= (fifo.buffer[0] << 0) & 0x7ff;
      status |= (fifo.buffer[0] << 4) & 0x8000;

      textured_rectangle_x_flip = ((fifo.buffer[0] >> 12) & 1) != 0;
      textured_rectangle_y_flip = ((fifo.buffer[0] >> 13) & 1) != 0;
      break;

    case 0xe2:
      texture_window_mask_x = utility::uclip<5>(fifo.buffer[0] >> 0);
      texture_window_mask_y = utility::uclip<5>(fifo.buffer[0] >> 5);
      texture_window_offset_x = utility::uclip<5>(fifo.buffer[0] >> 10);
      texture_window_offset_y = utility::uclip<5>(fifo.buffer[0] >> 15);
      break;

    case 0xe3:
      drawing_area_x1 = (fifo.buffer[0] >>  0) & 0x3ff;
      drawing_area_y1 = (fifo.buffer[0] >> 10) & 0x3ff;
      break;

    case 0xe4:
      drawing_area_x2 = (fifo.buffer[0] >>  0) & 0x3ff;
      drawing_area_y2 = (fifo.buffer[0] >> 10) & 0x3ff;
      break;

    case 0xe5:
      x_offset = utility::sclip<11>(fifo.buffer[0] >> 0);
      y_offset = utility::sclip<11>(fifo.buffer[0] >> 11);
      break;

    case 0xe6:
      status &= ~0x1800;
      status |= (fifo.buffer[0] << 11) & 0x1800;
      break;

    default:
      if (command_size[command] == 1) {
        printf("gpu::gp0(0x%08x)\n", fifo.buffer[0]);
      }
      break;
    }
  }
}
