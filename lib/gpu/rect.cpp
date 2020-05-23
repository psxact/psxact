#include "gpu/core.hpp"

#include "util/int.hpp"
#include "util/uint.hpp"

using namespace psx::gpu;
using namespace psx::util;

static int32_t get_x_length(psx::util::fifo< uint32_t, 4 > &fifo) {
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

static int32_t get_y_length(psx::util::fifo< uint32_t, 4 > &fifo) {
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

void core::draw_color(gp0_command command, const color &shade, const point &point, const texture_coord &coord, const texture_params &tev) {
  color color;
  bool blend;

  if (!command.is_texture_mapped()) {
    color = shade;
    blend = command.is_semi_transparent();
  } else {
    auto tex_color = get_texture_color(tev, coord);
    if (tex_color.is_full_transparent()) {
      return;
    }

    color = tex_color.to_color();
    blend = command.is_semi_transparent() && tex_color.is_semi_transparent();

    if (!command.is_raw_texture()) {
      color.r = std::min(0xff, (color.r * shade.r) / 0x80);
      color.g = std::min(0xff, (color.g * shade.g) / 0x80);
      color.b = std::min(0xff, (color.b * shade.b) / 0x80);
    }
  }

  if (blend) {
    auto cur_color = color::from_uint16(vram_read(point.x, point.y));

    switch (tev.color_mix_mode) {
      case 0:
        color.r = (cur_color.r + color.r) / 2;
        color.g = (cur_color.g + color.g) / 2;
        color.b = (cur_color.b + color.b) / 2;
        break;

      case 1:
        color.r = uint_t<8>::clamp(cur_color.r + color.r);
        color.g = uint_t<8>::clamp(cur_color.g + color.g);
        color.b = uint_t<8>::clamp(cur_color.b + color.b);
        break;

      case 2:
        color.r = uint_t<8>::clamp(cur_color.r - color.r);
        color.g = uint_t<8>::clamp(cur_color.g - color.g);
        color.b = uint_t<8>::clamp(cur_color.b - color.b);
        break;

      case 3:
        color.r = uint_t<8>::clamp(cur_color.r + (color.r / 4));
        color.g = uint_t<8>::clamp(cur_color.g + (color.g / 4));
        color.b = uint_t<8>::clamp(cur_color.b + (color.b / 4));
        break;
    }
  }

  draw_point(point, color);
}

void core::draw_rectangle() {
  texture_coord coord;
  texture_params params;

  auto command = gp0_command(fifo.at(0));
  auto shade = color::from_uint24(fifo.at(0));

  int32_t xofs = x_offset + int_t<11>::trunc(fifo.at(1));
  int32_t yofs = y_offset + int_t<11>::trunc(fifo.at(1) >> 16);

  if (command.is_texture_mapped()) {
    params.palette_page_x = (fifo.at(2) >> 12) & 0x3f0;
    params.palette_page_y = (fifo.at(2) >> 22) & 0x1ff;
    params.texture_page_x = (status << 6) & 0x3c0;
    params.texture_page_y = (status << 4) & 0x100;
    params.texture_colors = (status >> 7) & 3;
    params.color_mix_mode = (status >> 5) & 3;
    coord = texture_coord::from_uint16(fifo.at(2));
  }

  int32_t w = get_x_length(fifo);
  int32_t h = get_y_length(fifo);

  for (int32_t y = 0; y < h; y++) {
    for (int32_t x = 0; x < w; x++) {
      texture_coord this_coord;
      this_coord.u = coord.u + x;
      this_coord.v = coord.v + y;

      point point;
      point.x = xofs + x;
      point.y = yofs + y;

      draw_color(command, shade, point, this_coord, params);
    }
  }
}
