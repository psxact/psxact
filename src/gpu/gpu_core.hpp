#ifndef PSXACT_GPU_CORE_HPP
#define PSXACT_GPU_CORE_HPP

#include <cstdint>
#include <queue>

namespace gpu {
  struct state_t {
    uint32_t status;
    bool textured_rectangle_x_flip;
    bool textured_rectangle_y_flip;

    std::queue<uint32_t> gp0_fifo;
    std::queue<uint32_t> gp1_fifo;

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

    uint32_t gp0_texture_upload_size;
    uint32_t gp0_texture_download_size;
  };

  uint32_t read(int size, uint32_t address);

  void write(int size, uint32_t address, uint32_t data);
}

#endif //PSXACT_GPU_CORE_HPP
