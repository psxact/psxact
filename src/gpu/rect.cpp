// Copyright 2018 psxact

#include "gpu/core.hpp"

#include <algorithm>

using psx::gpu::core_t;

// Rect Commands
//
// 25    | Semi Transparency (0=Off, 1=On)

static int32_t get_x_length(uint32_t *fifo) {
  switch ((fifo[0] >> 27) & 3) {
  case 0:
    return
      (fifo[0] & (1 << 26))
        ? uint16_t(fifo[3])
        : uint16_t(fifo[2]);

  case 1:
    return 1;

  case 2:
    return 8;

  case 3:
    return 16;

  default:
    return 0;
  }
}

static int32_t get_y_length(uint32_t *fifo) {
  switch ((fifo[0] >> 27) & 3) {
  case 0:
    return
      (fifo[0] & (1 << 26))
        ? uint16_t(fifo[3] >> 16)
        : uint16_t(fifo[2] >> 16);

  case 1:
    return 1;

  case 2:
    return 8;

  case 3:
    return 16;

  default:
    return 0;
  }
}

bool core_t::get_color(uint32_t command, color_t *color, const tev_t &tev, const point_t &coord) {
  bool blended = (command & (1 << 24)) != 0;
  bool textured = (command & (1 << 26)) != 0;

  if (!textured) {
    return true;
  }

  color_t pixel = get_texture_color(tev, coord);

  if (blended) {
    color->r = std::min(255, (pixel.r * color->r) / 2);
    color->g = std::min(255, (pixel.g * color->g) / 2);
    color->b = std::min(255, (pixel.b * color->b) / 2);
  } else {
    color->r = pixel.r;
    color->g = pixel.g;
    color->b = pixel.b;
  }

  return (color->r | color->g | color->b) > 0;
}

void core_t::draw_rectangle() {
  tev_t tev;
  tev.palette_page_x = (fifo.buffer[2] >> 12) & 0x3f0;
  tev.palette_page_y = (fifo.buffer[2] >> 22) & 0x1ff;
  tev.texture_page_x = (status << 6) & 0x3c0;
  tev.texture_page_y = (status << 4) & 0x100;
  tev.texture_colors = (status >> 7) & 3;

  color_t color;
  color.r = (fifo.buffer[0] >> (0 * 8)) & 0xff;
  color.g = (fifo.buffer[0] >> (1 * 8)) & 0xff;
  color.b = (fifo.buffer[0] >> (2 * 8)) & 0xff;

  point_t tex_coord;
  tex_coord.x = (fifo.buffer[2] >> 0) & 0xff;
  tex_coord.y = (fifo.buffer[2] >> 8) & 0xff;

  int32_t xofs = x_offset + int16_t(fifo.buffer[1]);
  int32_t yofs = y_offset + int16_t(fifo.buffer[1] >> 16);

  int32_t w = get_x_length(fifo.buffer);
  int32_t h = get_y_length(fifo.buffer);

  for (int32_t y = 0; y < h; y++) {
    for (int32_t x = 0; x < w; x++) {
      point_t coord;
      coord.x = tex_coord.x + x;
      coord.y = tex_coord.y + y;

      if (get_color(fifo.buffer[0], &color, tev, coord)) {
        point_t point;
        point.x = xofs + x;
        point.y = yofs + y;

        draw_point(point, color);
      }
    }
  }
}
