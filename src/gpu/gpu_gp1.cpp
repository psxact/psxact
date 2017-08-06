#include "gpu_core.hpp"

void gpu::gp1(gpu_state_t &state, uint32_t data) {
  switch ((data >> 24) & 0x3f) {
  case 0x00:
    state.status = 0x14802000;
    state.textured_rectangle_x_flip = 0;
    state.textured_rectangle_y_flip = 0;
    break;

  case 0x01:
    state.fifo.wr = 0;
    state.fifo.rd = 0;
    break;

  case 0x02:
    state.status &= ~0x01000000;
    break;

  case 0x03:
    state.status &= ~0x00800000;
    state.status |= (data << 23) & 0x00800000;
    break;

  case 0x04:
    state.status &= ~0x60000000;
    state.status |= (data << 29) & 0x60000000;
    break;

  case 0x05:
    state.display_area_x = utility::uclip<10>(data >> 0);
    state.display_area_y = utility::uclip< 9>(data >> 10);
    break;

  case 0x06:
    state.display_area_x1 = utility::uclip<12>(data >> 0);
    state.display_area_x2 = utility::uclip<12>(data >> 12);
    break;

  case 0x07:
    state.display_area_y1 = utility::uclip<10>(data >> 0);
    state.display_area_y2 = utility::uclip<10>(data >> 10);
    break;

  case 0x08:
    state.status &= ~0x7f4000;
    state.status |= (data << 17) & 0x7e0000;
    state.status |= (data << 10) & 0x10000;
    state.status |= (data <<  7) & 0x4000;
    break;

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
      state.data_latch = (state.drawing_area_x1 & 0x3ff) | ((state.drawing_area_y1 & 0x3ff) << 10);
      break;

    case 0x04:
      state.data_latch = (state.drawing_area_x2 & 0x3ff) | ((state.drawing_area_y2 & 0x3ff) << 10);
      break;

    case 0x05:
      state.data_latch = (state.x_offset & 0x7ff) | ((state.y_offset & 0x7ff) << 11);
      break;

    case 0x07:
      state.data_latch = 2;
      break;

    default:
      printf("gpu::gp1(0x%08x)\n", data);
      break;
    }
    break;

  default:
    printf("gpu::gp1(0x%08x)\n", data);
    break;
  }
}
