#ifndef PSXACT_GPU_CORE_HPP
#define PSXACT_GPU_CORE_HPP

#include <cstdint>
#include <queue>
#include "../utility.hpp"

namespace gpu {
  union vram_t {
    uint8_t  b[utility::mib<1>() / 1];
    uint16_t h[utility::mib<1>() / 2];
    uint32_t w[utility::mib<1>() / 4];
  };

  extern vram_t vram;

  struct state_t {
    uint32_t status;
    bool textured_rectangle_x_flip;
    bool textured_rectangle_y_flip;

    std::deque<uint32_t> fifo;

    uint32_t gp0_command;
    uint32_t texture_window_mask_x;
    uint32_t texture_window_mask_y;
    uint32_t texture_window_offset_x;
    uint32_t texture_window_offset_y;
    uint32_t drawing_area_x1;
    uint32_t drawing_area_y1;
    uint32_t drawing_area_x2;
    uint32_t drawing_area_y2;
    uint32_t x_offset;
    uint32_t y_offset;
    uint32_t display_area_x;
    uint32_t display_area_y;
    uint32_t display_area_x1;
    uint32_t display_area_y1;
    uint32_t display_area_x2;
    uint32_t display_area_y2;

    struct {
      int remaining;
      int x;
      int y;
      int w;
      int h;
      int current_x;
      int current_y;
    } texture_upload;

    struct {
      int remaining;
      int x;
      int y;
      int w;
      int h;
      int current_x;
      int current_y;
    } texture_download;
  };

  uint32_t mmio_read(int size, uint32_t address);

  void mmio_write(int size, uint32_t address, uint32_t data);

  uint16_t read_vram(int x, int y);

  void write_vram(int x, int y, uint16_t color);

  struct color_t {
    int r;
    int g;
    int b;
  };

  struct point_t {
    int x;
    int y;
  };

  void draw_point(int x, int y, int r, int g, int b);

  namespace gouraud {
    struct pixel_t {
      point_t point;
      color_t color;
    };

    template<int size>
    struct polygon_t {
      pixel_t v[size];
    };

    void draw_poly3(const gpu::gouraud::polygon_t<3> &p);

    void draw_poly4(const gpu::gouraud::polygon_t<4> &p);
  }

  namespace texture {
    struct pixel_t {
      point_t point;
      color_t color;
      int u;
      int v;
    };

    template<int size>
    struct polygon_t {
      pixel_t v[size];
      int clut_x;
      int clut_y;
      int base_u;
      int base_v;
    };

    void draw_poly3(const polygon_t<3> &p);

    void draw_poly4(const polygon_t<4> &p);
  }
}

#endif //PSXACT_GPU_CORE_HPP
