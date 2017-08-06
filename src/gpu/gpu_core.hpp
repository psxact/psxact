#ifndef __PSXACT_GPU_CORE_HPP__
#define __PSXACT_GPU_CORE_HPP__

#include <queue>
#include "../bus.hpp"
#include "../state.hpp"

namespace gpu {
  uint32_t io_read(gpu_state_t &state, bus::bus_width_t width, uint32_t address);

  void io_write(gpu_state_t &state, bus::bus_width_t width, uint32_t address, uint32_t data);

  uint32_t data(gpu_state_t &state);

  uint32_t stat(gpu_state_t &state);

  void gp0(gpu_state_t &state, uint32_t data);

  void gp1(gpu_state_t &state, uint32_t data);

  void vram_transfer(gpu_state_t &state, uint16_t data);

  uint16_t vram_transfer(gpu_state_t &state);

  struct color_t {
    uint8_t r;
    uint8_t g;
    uint8_t b;
  };

  struct point_t {
    int32_t x;
    int32_t y;
  };

  struct tev_t {
    // from 'palette'
    int32_t palette_page_x;
    int32_t palette_page_y;

    // from 'texpage'
    int32_t texture_colors;
    int32_t texture_page_x;
    int32_t texture_page_y;
    int32_t color_mix_mode;
  };

  void draw_point(gpu_state_t &state, point_t point, color_t color);

  void draw_line(gpu_state_t &state);

  void draw_polygon(gpu_state_t &state);

  void draw_rectangle(gpu_state_t &state);

  // common functionality

  color_t uint16_to_color(uint16_t value);

  uint16_t color_to_uint16(color_t color);

  color_t get_texture_color__4bpp(gpu::tev_t &tev, gpu::point_t &coord);

  color_t get_texture_color__8bpp(gpu::tev_t &tev, gpu::point_t &coord);

  color_t get_texture_color_15bpp(gpu::tev_t &tev, gpu::point_t &coord);

  color_t get_texture_color(gpu::tev_t &tev, gpu::point_t &coord);
}

#endif // __PSXACT_GPU_CORE_HPP__
