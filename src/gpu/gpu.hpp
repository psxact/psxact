#ifndef __psxact_gpu_core_hpp__
#define __psxact_gpu_core_hpp__


#include "console.hpp"


struct gpu_t {
  uint32_t data_latch = 0;
  uint32_t status = 0x14802000;
  uint32_t texture_window_mask_x;
  uint32_t texture_window_mask_y;
  uint32_t texture_window_offset_x;
  uint32_t texture_window_offset_y;
  int32_t drawing_area_x1;
  int32_t drawing_area_y1;
  int32_t drawing_area_x2;
  int32_t drawing_area_y2;
  int32_t x_offset;
  int32_t y_offset;
  uint32_t display_area_x;
  uint32_t display_area_y;
  uint32_t display_area_x1;
  uint32_t display_area_y1;
  uint32_t display_area_x2;
  uint32_t display_area_y2;
  bool textured_rectangle_x_flip;
  bool textured_rectangle_y_flip;

  struct {
    uint32_t buffer[16];
    int32_t wr;
    int32_t rd;
  } fifo;

  struct {
    struct {
      int32_t x;
      int32_t y;
      int32_t w;
      int32_t h;
    } reg;

    struct {
      bool active;
      int32_t x;
      int32_t y;
    } run;
  } cpu_to_gpu_transfer;

  struct {
    struct {
      int32_t x;
      int32_t y;
      int32_t w;
      int32_t h;
    } reg;

    struct {
      bool active;
      int32_t x;
      int32_t y;
    } run;
  } gpu_to_cpu_transfer;

  uint32_t io_read(bus_width_t width, uint32_t address);

  void io_write(bus_width_t width, uint32_t address, uint32_t data);

  uint32_t data();

  uint32_t stat();

  void gp0(uint32_t data);

  void gp1(uint32_t data);

  void vram_transfer(uint16_t data);

  uint16_t vram_transfer();

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

  void copy_vram_to_vram();

  void copy_wram_to_vram();

  void copy_vram_to_wram();

  void draw_point(point_t point, color_t color);

  void draw_line();

  void draw_polygon();

  void draw_rectangle();

  void fill_rectangle();

  // common functionality

  static color_t uint16_to_color(uint16_t value);

  static uint16_t color_to_uint16(color_t color);

  static color_t get_texture_color__4bpp(tev_t &tev, point_t &coord);

  static color_t get_texture_color__8bpp(tev_t &tev, point_t &coord);

  static color_t get_texture_color_15bpp(tev_t &tev, point_t &coord);

  static color_t get_texture_color(tev_t &tev, point_t &coord);
};


#endif // __psxact_gpu_core_hpp__
