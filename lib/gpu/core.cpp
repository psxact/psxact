#include "gpu/core.hpp"

#include <cassert>

using namespace psx::gpu;

core_t::core_t(irq_line_t irq, bool log_enabled)
  : addressable_t("gpu", log_enabled)
  , irq(irq) {
  vram = new memory_t< mib(1) >("vram");
}

core_t::~core_t() {
  delete vram;
}

bool core_t::run(int amount) {
  // time-base conversion
  prescaler += amount * 11;
  int ticks = prescaler / 7;
  prescaler = prescaler % 7;

  // the real update function
  return tick(ticks);
}

bool core_t::tick(int amount) {
  constexpr int VBLANK_START = 241 * 3413;
  constexpr int VBLANK_END = 263 * 3413;

  assert(amount < VBLANK_END);

  int prev = counter;
  int next = counter + amount;

  counter = next % VBLANK_END;

  if (prev < VBLANK_START && next >= VBLANK_START) {
    irq(irq_line_state_t::active);
  }

  if (prev < VBLANK_END && next >= VBLANK_END) {
    irq(irq_line_state_t::clear);
  }

  return next >= VBLANK_END;
}

int core_t::dma_speed() {
  return 1;
}

bool core_t::dma_ready() {
  return true;
}

uint32_t core_t::dma_read() {
  return data();
}

void core_t::dma_write(uint32_t val) {
  gp0(val);
}

uint32_t core_t::data() {
  if (gpu_to_cpu_transfer.run.active) {
    uint16_t lower = vram_transfer_read();
    uint16_t upper = vram_transfer_read();

    return (upper << 16) | lower;
  }

  return data_latch;
}

uint32_t core_t::stat() {
  //  19    Vertical Resolution         (0=240, 1=480, when Bit22=1)  ;GP1(08h).2
  //  26    Ready to receive Cmd Word   (0=No, 1=Ready)  ;GP0(...) ;via GP0
  //  27    Ready to send VRAM to CPU   (0=No, 1=Ready)  ;GP0(C0h) ;via GPUREAD
  //  28    Ready to receive DMA Block  (0=No, 1=Ready)  ;GP0(...) ;via GP0

  return (status & ~0x00080000) | 0x1c002000;
}

uint32_t core_t::io_read_word(uint32_t address) {
  switch (address) {
    case GPU_READ:
      return data();

    case GPU_STAT:
      return stat();
  }

  return 0;
}

void core_t::io_write_word(uint32_t address, uint32_t data) {
  switch (address) {
    case GPU_GP0:
      return gp0(data);

    case GPU_GP1:
      return gp1(data);
  }
}

// common functionality

core_t::color_t core_t::uint16_to_color(uint16_t value) {
  color_t color;
  color.r = (value << 3) & 0xf8;
  color.g = (value >> 2) & 0xf8;
  color.b = (value >> 7) & 0xf8;

  return color;
}

uint16_t core_t::color_to_uint16(const color_t &color) {
  return
    ((color.r >> 3) & 0x001f) |
    ((color.g << 2) & 0x03e0) |
    ((color.b << 7) & 0x7c00);
}

core_t::color_t core_t::get_texture_color__4bpp(const tev_t &tev, const point_t &coord) {
  uint16_t texel = vram_read(
    tev.texture_page_x + coord.x / 4,
    tev.texture_page_y + coord.y);

  texel = (texel >> ((coord.x & 3) * 4)) & 15;

  uint16_t pixel = vram_read(
    tev.palette_page_x + texel,
    tev.palette_page_y);

  return uint16_to_color(pixel);
}

core_t::color_t core_t::get_texture_color__8bpp(const tev_t &tev, const point_t &coord) {
  uint16_t texel = vram_read(
    tev.texture_page_x + coord.x / 2,
    tev.texture_page_y + coord.y);

  texel = (texel >> ((coord.x & 1) * 8)) & 255;

  uint16_t pixel = vram_read(
    tev.palette_page_x + texel,
    tev.palette_page_y);

  return uint16_to_color(pixel);
}

core_t::color_t core_t::get_texture_color_15bpp(const tev_t &tev, const point_t &coord) {
  uint16_t pixel = vram_read(
    tev.texture_page_x + coord.x,
    tev.texture_page_y + coord.y);

  return uint16_to_color(pixel);
}

core_t::color_t core_t::get_texture_color(const tev_t &tev, const point_t &coord) {
  switch (tev.texture_colors) {
  default:
  case 0:
    return get_texture_color__4bpp(tev, coord);

  case 1:
    return get_texture_color__8bpp(tev, coord);

  case 2:
    return get_texture_color_15bpp(tev, coord);

  case 3:
    return get_texture_color_15bpp(tev, coord);
  }
}
