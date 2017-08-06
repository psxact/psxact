#include <cassert>
#include "gpu_core.hpp"
#include "../bus.hpp"
#include "../memory/vram.hpp"

uint32_t gpu::data(gpu_state_t &state) {
  if (state.gpu_to_cpu_transfer.run.active) {
    uint16_t lower = vram_transfer(state);
    uint16_t upper = vram_transfer(state);

    return (upper << 16) | lower;
  }

  return state.data_latch;
}

uint32_t gpu::stat(gpu_state_t &state) {
  //  19    Vertical Resolution         (0=240, 1=480, when Bit22=1)  ;GP1(08h).2
  //  26    Ready to receive Cmd Word   (0=No, 1=Ready)  ;GP0(...) ;via GP0
  //  27    Ready to send VRAM to CPU   (0=No, 1=Ready)  ;GP0(C0h) ;via GPUREAD
  //  28    Ready to receive DMA Block  (0=No, 1=Ready)  ;GP0(...) ;via GP0

  return (state.status & ~0x00080000) | 0x1c002000;
}

uint32_t gpu::io_read(gpu_state_t &state, bus_width_t width, uint32_t address) {
  assert(width == BUS_WIDTH_WORD);

  if (utility::log_gpu) {
    printf("gpu::io_read(%d, 0x%08x)\n", width, address);
  }

  switch (address) {
  case 0x1f801810:
    return data(state);

  case 0x1f801814:
    return stat(state);

  default:
    return 0;
  }
}

void gpu::io_write(gpu_state_t &state, bus_width_t width, uint32_t address, uint32_t data) {
  assert(width == BUS_WIDTH_WORD);

  if (utility::log_gpu) {
    printf("gpu::io_write(%d, 0x%08x, 0x%08x)\n", width, address, data);
  }

  switch (address) {
  case 0x1f801810:
    return gp0(state, data);

  case 0x1f801814:
    return gp1(state, data);
  }
}

uint16_t gpu::vram_transfer(gpu_state_t &state) {
  auto &transfer = state.gpu_to_cpu_transfer;
  if (!transfer.run.active) {
    return 0;
  }

  uint16_t data = vram::read(
    transfer.reg.x + transfer.run.x,
    transfer.reg.y + transfer.run.y);

  transfer.run.x++;

  if (transfer.run.x == transfer.reg.w) {
    transfer.run.x = 0;
    transfer.run.y++;

    if (transfer.run.y == transfer.reg.h) {
      transfer.run.y = 0;
      transfer.run.active = false;
    }
  }

  return data;
}

void gpu::vram_transfer(gpu_state_t &state, uint16_t data) {
  auto &transfer = state.cpu_to_gpu_transfer;
  if (!transfer.run.active) {
    return;
  }

  vram::write(
    transfer.reg.x + transfer.run.x,
    transfer.reg.y + transfer.run.y, uint16_t(data));

  transfer.run.x++;

  if (transfer.run.x == transfer.reg.w) {
    transfer.run.x = 0;
    transfer.run.y++;

    if (transfer.run.y == transfer.reg.h) {
      transfer.run.y = 0;
      transfer.run.active = false;
    }
  }
}

// common functionality

gpu::color_t gpu::uint16_to_color(uint16_t value) {
  color_t color;
  color.r = (value << 3) & 0xf8;
  color.g = (value >> 2) & 0xf8;
  color.b = (value >> 7) & 0xf8;

  return color;
}

uint16_t gpu::color_to_uint16(color_t color) {
  return
    ((color.r >> 3) & 0x001f) |
    ((color.g << 2) & 0x03e0) |
    ((color.b << 7) & 0x7c00);
}

gpu::color_t gpu::get_texture_color__4bpp(gpu::tev_t &tev, gpu::point_t &coord) {
  uint16_t texel = vram::read(tev.texture_page_x + coord.x / 4,
                              tev.texture_page_y + coord.y);

  texel = (texel >> ((coord.x & 3) * 4)) & 15;

  uint16_t pixel = vram::read(tev.palette_page_x + texel,
                              tev.palette_page_y);

  return uint16_to_color(pixel);
}

gpu::color_t gpu::get_texture_color__8bpp(gpu::tev_t &tev, gpu::point_t &coord) {
  uint16_t texel = vram::read(tev.texture_page_x + coord.x / 2,
                              tev.texture_page_y + coord.y);

  texel = (texel >> ((coord.x & 1) * 8)) & 255;

  uint16_t pixel = vram::read(tev.palette_page_x + texel,
                              tev.palette_page_y);

  return uint16_to_color(pixel);
}

gpu::color_t gpu::get_texture_color_15bpp(gpu::tev_t &tev, gpu::point_t &coord) {
  uint16_t pixel = vram::read(tev.texture_page_x + coord.x,
                              tev.texture_page_y + coord.y);

  return uint16_to_color(pixel);
}

gpu::color_t gpu::get_texture_color(gpu::tev_t &tev, gpu::point_t &coord) {
  switch (tev.texture_colors) {
  default:

  case 0:
    return gpu::get_texture_color__4bpp(tev, coord);

  case 1:
    return gpu::get_texture_color__8bpp(tev, coord);

  case 2:
    return gpu::get_texture_color_15bpp(tev, coord);

  case 3:
    return gpu::get_texture_color_15bpp(tev, coord);
  }
}
