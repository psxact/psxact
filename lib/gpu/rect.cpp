#include "gpu/core.hpp"

#include <algorithm>

using namespace psx::gpu;

static int32_t get_x_length(psx::util::fifo_t< uint32_t, 4 > &fifo) {
  switch ((fifo.at(0) >> 27) & 3) {
  case  1: return 1;
  case  2: return 8;
  case  3: return 16;
  default:
    return (fifo.at(0) & (1 << 26))
      ? uint16_t(fifo.at(3))
      : uint16_t(fifo.at(2));
  }
}

static int32_t get_y_length(psx::util::fifo_t< uint32_t, 4 > &fifo) {
  switch ((fifo.at(0) >> 27) & 3) {
  case  1: return 1;
  case  2: return 8;
  case  3: return 16;
  default:
    return (fifo.at(0) & (1 << 26))
      ? uint16_t(fifo.at(3) >> 16)
      : uint16_t(fifo.at(2) >> 16);
  }
}

bool core_t::get_color(gp0_command_t command, const color_t &shade, const texture_coord_t &coord, const tev_t &tev, color_t *out_color) {
  if (command.is_texture_mapped()) {
    auto color = get_texture_color(tev, coord);

    if (command.is_raw_texture()) {
      *out_color = color;
    } else {
      out_color->r = std::min(255, (color.r * shade.r) / 128);
      out_color->g = std::min(255, (color.g * shade.g) / 128);
      out_color->b = std::min(255, (color.b * shade.b) / 128);
    }
  } else {
    *out_color = shade;
  }

  return (out_color->r | out_color->g | out_color->b) > 0;
}

void core_t::draw_rectangle() {
  tev_t tev;
  tev.palette_page_x = (fifo.at(2) >> 12) & 0x3f0;
  tev.palette_page_y = (fifo.at(2) >> 22) & 0x1ff;
  tev.texture_page_x = (status << 6) & 0x3c0;
  tev.texture_page_y = (status << 4) & 0x100;
  tev.texture_colors = (status >> 7) & 3;

  auto command = gp0_command_t(fifo.at(0));
  auto shade = color_t::from_uint24(fifo.at(0));
  auto coord = texture_coord_t::from_uint16(fifo.at(2));

  int32_t xofs = x_offset + int16_t(fifo.at(1));
  int32_t yofs = y_offset + int16_t(fifo.at(1) >> 16);

  int32_t w = get_x_length(fifo);
  int32_t h = get_y_length(fifo);

  for (int32_t y = 0; y < h; y++) {
    for (int32_t x = 0; x < w; x++) {
      texture_coord_t this_coord;
      this_coord.u = coord.u + x;
      this_coord.v = coord.v + y;

      color_t color;
      if (get_color(command, shade, this_coord, tev, &color)) {
        point_t point;
        point.x = xofs + x;
        point.y = yofs + y;

        if (command.is_semi_transparent()) {
          assert(0 && "Semi-transparency isn't implemented for rectangles yet.");
        }

        draw_point(point, color);
      }
    }
  }
}
