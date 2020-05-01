#include "gpu/core.hpp"

#include "util/int.hpp"
#include "util/uint.hpp"

using namespace psx::gpu;
using namespace psx::util;

static int command_size[256] = {
  1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // $00
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // $10
  4, 4, 4, 4, 7, 7, 7, 7, 5, 5, 5, 5, 9, 9, 9, 9,  // $20
  6, 6, 6, 6, 9, 9, 9, 9, 8, 8, 8, 8,12,12,12,12,  // $30

  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  // $40
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  // $50
  3, 3, 3, 3, 4, 4, 4, 4, 2, 2, 2, 2, 3, 3, 3, 3,  // $60
  2, 2, 2, 2, 3, 3, 3, 3, 2, 2, 2, 2, 3, 3, 3, 3,  // $70

  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  // $80
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  // $90
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  // $a0
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  // $b0

  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  // $c0
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  // $d0
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // $e0
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // $f0
};

void core_t::fill_rectangle() {
  assert(fifo.size() == 3);

  uint16_t color =
    ((fifo.at(0) >> 3) & 0x001f) |
    ((fifo.at(0) >> 6) & 0x03e0) |
    ((fifo.at(0) >> 9) & 0x7c00);

  point_t point;
  point.x = (fifo.at(1) + 0x0) & 0x3f0;
  point.y = (fifo.at(1) >> 16) & 0x1ff;

  point_t count;
  count.x = (fifo.at(2) + 0xf) & 0x7f0;
  count.y = (fifo.at(2) >> 16) & 0x1ff;

  for (int y = 0; y < count.y; y++) {
    for (int x = 0; x < count.x; x++) {
      vram_write(
        point.x + x,
        point.y + y,
        color);
    }
  }
}

void core_t::copy_vram_to_vram() {}

void core_t::copy_wram_to_vram() {
  assert(fifo.size() == 3);

  auto &transfer = cpu_to_gpu_transfer;
  transfer.reg.x = fifo.at(1) & 0xffff;
  transfer.reg.y = fifo.at(1) >> 16;
  transfer.reg.w = fifo.at(2) & 0xffff;
  transfer.reg.h = fifo.at(2) >> 16;

  transfer.run.x = 0;
  transfer.run.y = 0;
  transfer.run.active = true;
}

void core_t::copy_vram_to_wram() {
  assert(fifo.size() == 3);

  auto &transfer = gpu_to_cpu_transfer;
  transfer.reg.x = fifo.at(1) & 0xffff;
  transfer.reg.y = fifo.at(1) >> 16;
  transfer.reg.w = fifo.at(2) & 0xffff;
  transfer.reg.h = fifo.at(2) >> 16;

  transfer.run.x = 0;
  transfer.run.y = 0;
  transfer.run.active = true;
}

void core_t::gp0(uint32_t data) {
  log("gp0(%08x)", data);

  if (cpu_to_gpu_transfer.run.active) {
    uint16_t lower = uint16_t(data >> 0);
    uint16_t upper = uint16_t(data >> 16);

    vram_transfer_write(lower);
    vram_transfer_write(upper);
    return;
  }

  fifo.write(data);

  uint32_t command = fifo.at(0) >> 24;

  if (fifo.size() == command_size[command]) {
    run_command();
    fifo.clear();
  }
}

void core_t::run_command() {
  uint32_t command = fifo.at(0) >> 24;

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
    case 0x00:  // nop
      break;

    case 0x01:  // clear texture cache
      break;

    case 0x02:
      return fill_rectangle();

    case 0x1F:
      status |= (1 << 24);
      return;

    case 0xe1:
      status &= ~0x87ff;
      status |= (fifo.at(0) << 0) & 0x7ff;
      status |= (fifo.at(0) << 4) & 0x8000;

      textured_rectangle_x_flip = ((fifo.at(0) >> 12) & 1) != 0;
      textured_rectangle_y_flip = ((fifo.at(0) >> 13) & 1) != 0;
      break;

    case 0xe2:
      texture_window_mask_x = uint_t<5>::trunc(fifo.at(0) >> 0);
      texture_window_mask_y = uint_t<5>::trunc(fifo.at(0) >> 5);
      texture_window_offset_x = uint_t<5>::trunc(fifo.at(0) >> 10);
      texture_window_offset_y = uint_t<5>::trunc(fifo.at(0) >> 15);
      break;

    case 0xe3:
      assert(fifo.size() == 1);
      drawing_area_x1 = (fifo.at(0) >> 0) & 0x3ff;
      drawing_area_y1 = (fifo.at(0) >> 10) & 0x3ff;
      break;

    case 0xe4:
      drawing_area_x2 = (fifo.at(0) >> 0) & 0x3ff;
      drawing_area_y2 = (fifo.at(0) >> 10) & 0x3ff;
      break;

    case 0xe5:
      x_offset = int_t<11>::trunc(fifo.at(0) >> 0);
      y_offset = int_t<11>::trunc(fifo.at(0) >> 11);
      break;

    case 0xe6:
      status &= ~0x1800;
      status |= (fifo.at(0) << 11) & 0x1800;
      break;
  }
}
