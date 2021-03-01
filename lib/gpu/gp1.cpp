#include "gpu/core.hpp"

#include "util/uint.hpp"

using namespace psx::gpu;
using namespace psx::util;

void core::gp1(uint32_t data) {
  LOG_INFO("gp1(%08x)", data);

  switch ((data >> 24) & 0x3f) {
    case 0x00:
      status = 0x14802000;
      gpu_to_cpu_transfer.run.active = false;
      textured_rectangle_x_flip = 0;
      textured_rectangle_y_flip = 0;
      break;

    case 0x01:
      fifo.clear();
      break;

    case 0x02:
      irq(wire_state::off);
      status &= ~(1 << 24);
      break;

    case 0x03:
      status &= ~0x00800000;
      status |= (data << 23) & 0x00800000;
      break;

    case 0x04:
      status &= ~0x60000000;
      status |= (data << 29) & 0x60000000;
      break;

    case 0x05:
      display_area_x = uint_t<10>::trunc(data >> 0);
      display_area_y = uint_t<9>::trunc(data >> 10);
      break;

    case 0x06:
      display_area_x1 = uint_t<12>::trunc(data >> 0);
      display_area_x2 = uint_t<12>::trunc(data >> 12);
      break;

    case 0x07:
      display_area_y1 = uint_t<10>::trunc(data >> 0);
      display_area_y2 = uint_t<10>::trunc(data >> 10);
      break;

    case 0x08: {
      if (data & (1 << 6)) {
        h_resolution = gpu_h_resolution::h368;
      } else {
        switch (data & (3 << 0)) {
          case 0: h_resolution = gpu_h_resolution::h256; break;
          case 1: h_resolution = gpu_h_resolution::h320; break;
          case 2: h_resolution = gpu_h_resolution::h512; break;
          case 3: h_resolution = gpu_h_resolution::h640; break;
        }
      }

      if ((data & (1 << 2)) && (data & (1 << 5))) {
        v_resolution = gpu_v_resolution::v480;
      } else {
        v_resolution = gpu_v_resolution::v240;
      }

      if (data & (1 << 4)) {
        display_depth = gpu_display_depth::bpp24;
      } else {
        display_depth = gpu_display_depth::bpp15;
      }

      status &= ~0x7f4000;
      status |= (data << 17) & 0x7e0000;
      status |= (data << 10) & 0x010000;
      status |= (data <<  7) & 0x004000;
      break;
    }

    case 0x10:
    case 0x11:
    case 0x12:
    case 0x13:
    case 0x14:
    case 0x15:
    case 0x16:
    case 0x17:
    case 0x18:
    case 0x19:
    case 0x1a:
    case 0x1b:
    case 0x1c:
    case 0x1d:
    case 0x1e:
    case 0x1f:
      switch (data & 0xf) {
        case 0x03:
          data_latch = (drawing_area_x1 & 0x3ff) | ((drawing_area_y1 & 0x3ff) << 10);
          break;

        case 0x04:
          data_latch = (drawing_area_x2 & 0x3ff) | ((drawing_area_y2 & 0x3ff) << 10);
          break;

        case 0x05:
          data_latch = (x_offset & 0x7ff) | ((y_offset & 0x7ff) << 11);
          break;

        case 0x07:
          data_latch = 2;
          break;
      }
      break;
  }
}
