#include "gpu/core.hpp"

#include <cassert>
#include "gpu/gamma.hpp"
#include "args.hpp"
#include "timing.hpp"

using namespace psx::gpu;
using namespace psx::util;

core_t::core_t(wire_t irq, wire_t hbl, wire_t vbl)
  : addressable_t("gpu", args::log_gpu)
  , irq(irq)
  , hbl(hbl)
  , vbl(vbl)
  , h_resolution(gpu_h_resolution_t::h256)
  , v_resolution(gpu_v_resolution_t::v240) {
  vram = new memory_t< mib(1) >("vram");
}

core_t::~core_t() {
  delete vram;
}

uint32_t *core_t::get_video_buffer() const {
  return (uint32_t *)video_buffer;
}

int32_t core_t::get_width() const {
  return 640;
}

int32_t core_t::get_height() const {
  return 480;
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
  constexpr int VBLANK_START = int32_t(241 * GPU_LINE_LENGTH + 0.5);
  constexpr int VBLANK_END = int32_t(262.5 * GPU_LINE_LENGTH + 0.5);

  assert(amount < VBLANK_END);

  int prev = counter;
  int next = counter + amount;

  counter = next % VBLANK_END;

  if (prev < VBLANK_START && next >= VBLANK_START) {
    vbl(wire_state_t::on);
  }

  if (prev < VBLANK_END && next >= VBLANK_END) {
    vbl(wire_state_t::off);
  }

  if (next >= VBLANK_END) {
    if (get_v_resolution() == gpu_v_resolution_t::v480) {
      render_field_480i();
      field = field == gpu_field_t::even ? gpu_field_t::odd : gpu_field_t::even;
    } else {
      render_field_240p();
      field = gpu_field_t::odd;
    }
    return true;
  }

  return false;
}

void core_t::render_field_240p() {
  const int width = int(get_h_resolution());
  const int height = 480;

  const int vscale = get_v_resolution() == gpu_v_resolution_t::v480 ? 1 : 2;

  if (get_display_depth() == gpu_display_depth_t::bpp24) {
    for (int y = 1; y < height; y += 2) {
      for (int x = 0; x < width; x++) {
        color_t color;
        color.r = vram_read8(x * 3 + 0, y / vscale);
        color.g = vram_read8(x * 3 + 1, y / vscale);
        color.b = vram_read8(x * 3 + 2, y / vscale);

        gamma_t::apply(color);

        video_buffer[y - 1][x] = 0;
        video_buffer[y - 0][x] = color.to_uint32();
      }
    }
  } else {
    for (int y = 1; y < height; y += 2) {
      for (int x = 0; x < width; x++) {
        uint16_t pixel = vram_read(
          display_area_x + x,
          display_area_y + (y / vscale));

        color_t color = color_t::from_uint16(pixel);

        gamma_t::apply(color);

        video_buffer[y - 1][x] = 0;
        video_buffer[y - 0][x] = color.to_uint32();
      }
    }
  }
}

void core_t::render_field_480i() {
  const int width = int(get_h_resolution());
  const int height = 480;

  const int field = int(this->field);

  if (get_display_depth() == gpu_display_depth_t::bpp24) {
    for (int y = field; y < height; y += 2) {
      for (int x = 0; x < width; x++) {
        color_t color;
        color.r = vram_read8(x * 3 + 0, y);
        color.g = vram_read8(x * 3 + 1, y);
        color.b = vram_read8(x * 3 + 2, y);

        gamma_t::apply(color);

        video_buffer[y][x] = color.to_uint32();
      }
    }
  } else {
    for (int y = field; y < height; y += 2) {
      for (int x = 0; x < width; x++) {
        uint16_t pixel = vram_read(
          display_area_x + x,
          display_area_y + y);

        color_t color = color_t::from_uint16(pixel);

        gamma_t::apply(color);

        video_buffer[y][x] = color.to_uint32();
      }
    }
  }
}

uint32_t core_t::get_status() const {
  return status;
}

gpu_h_resolution_t core_t::get_h_resolution() const {
  return h_resolution;
}

gpu_v_resolution_t core_t::get_v_resolution() const {
  return v_resolution;
}

gpu_display_depth_t core_t::get_display_depth() const {
  return display_depth;
}

int core_t::dma_speed() {
  return 1;
}

bool core_t::dma_read_ready() {
  return true;
}

bool core_t::dma_write_ready() {
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
  auto bit31 = int(field);
  auto bit13 = int(field);

  //  26    Ready to receive Cmd Word   (0=No, 1=Ready)  ;GP0(...) ;via GP0
  //  28    Ready to receive DMA Block  (0=No, 1=Ready)  ;GP0(...) ;via GP0

  return status | 0x14000000
      | (gpu_to_cpu_transfer.run.active << 27)
      | (bit31 << 31)
      | (bit13 << 13);
}

uint32_t core_t::io_read(address_width_t width, uint32_t address) {
  if (width == address_width_t::word) {
    switch (address) {
      case GPU_READ:
        return data();

      case GPU_STAT:
        return stat();
    }
  }

  return addressable_t::io_read(width, address);
}

void core_t::io_write(address_width_t width, uint32_t address, uint32_t data) {
  if (width == address_width_t::word) {
    switch (address) {
      case GPU_GP0:
        return gp0(data);

      case GPU_GP1:
        return gp1(data);
    }
  }

  return addressable_t::io_write(width, address, data);
}

// common functionality

color_t core_t::get_texture_color__4bpp(const tev_t &tev, const texture_coord_t &coord) {
  uint16_t texel = vram_read(
    tev.texture_page_x + (coord.u / 4),
    tev.texture_page_y + coord.v);

  texel = (texel >> ((coord.u & 3) * 4)) & 15;

  uint16_t pixel = vram_read(
    tev.palette_page_x + texel,
    tev.palette_page_y);

  return color_t::from_uint16(pixel);
}

color_t core_t::get_texture_color__8bpp(const tev_t &tev, const texture_coord_t &coord) {
  uint16_t texel = vram_read(
    tev.texture_page_x + (coord.u / 2),
    tev.texture_page_y + coord.v);

  texel = (texel >> ((coord.u & 1) * 8)) & 255;

  uint16_t pixel = vram_read(
    tev.palette_page_x + texel,
    tev.palette_page_y);

  return color_t::from_uint16(pixel);
}

color_t core_t::get_texture_color_15bpp(const tev_t &tev, const texture_coord_t &coord) {
  uint16_t pixel = vram_read(
    tev.texture_page_x + coord.u,
    tev.texture_page_y + coord.v);

  return color_t::from_uint16(pixel);
}

color_t core_t::get_texture_color(const tev_t &tev, const texture_coord_t &coord) {
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
