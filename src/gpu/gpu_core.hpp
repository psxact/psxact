#ifndef __PSXACT_GPU_CORE_HPP__
#define __PSXACT_GPU_CORE_HPP__

#include <cstdint>
#include <queue>
#include "../state.hpp"
#include "../utility.hpp"

namespace gpu {
  uint32_t io_read(gpu_state_t *state, int width, uint32_t address);

  void io_write(gpu_state_t *state, int width, uint32_t address, uint32_t data);

  uint32_t data(gpu_state_t *state);

  uint32_t stat(gpu_state_t *state);

  void gp0(gpu_state_t *state, uint32_t data);

  void gp1(gpu_state_t *state, uint32_t data);

  void vram_transfer(gpu_state_t *state, uint16_t data);

  uint16_t vram_transfer(gpu_state_t *state);

  struct color_t {
    int r;
    int g;
    int b;
  };

  struct point_t {
    int x;
    int y;
  };

  void draw_point(gpu_state_t *state, int x, int y, int r, int g, int b);

  void draw_rect(gpu_state_t *state);

  namespace gouraud {
    struct pixel_t {
      point_t point;
      color_t color;
    };

    template<int size>
    struct polygon_t {
      pixel_t v[size];
    };

    void draw_poly3(gpu_state_t *state, const gpu::gouraud::polygon_t<3> &p);

    void draw_poly4(gpu_state_t *state, const gpu::gouraud::polygon_t<4> &p);
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
      int depth;
    };

    void draw_poly3(gpu_state_t *state, const polygon_t<3> &p);

    void draw_poly4(gpu_state_t *state, const polygon_t<4> &p);
  }
}

#endif // __PSXACT_GPU_CORE_HPP__
