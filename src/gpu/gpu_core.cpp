#include "gpu_core.hpp"
#include "../utility.hpp"
#include <stdexcept>
#include <stdio.h>

static gpu::state_t state;

static int gp0_command_size[256] = {
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // $00
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // $10
  1, 1, 1, 1, 1, 1, 1, 1, 5, 1, 1, 1, 9, 1, 1, 1, // $20
  6, 1, 1, 1, 1, 1, 1, 1, 8, 1, 1, 1, 1, 1, 1, 1, // $30
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // $40
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // $50
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // $60
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // $70
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // $80
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // $90
  3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // $a0
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // $b0
  3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // $c0
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // $d0
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // $e0
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // $f0
};

static inline uint32_t read_resp() {
  if (state.gp0_texture_download_size) {
    state.gp0_texture_download_size--;
    return 0;
  }

  printf("gpu::read_resp()\n");
  return 0;
}

static inline uint32_t read_stat() {
  //  19    Vertical Resolution         (0=240, 1=480, when Bit22=1)  ;GP1(08h).2
  //  26    Ready to receive Cmd Word   (0=No, 1=Ready)  ;GP0(...) ;via GP0
  //  27    Ready to send VRAM to CPU   (0=No, 1=Ready)  ;GP0(C0h) ;via GPUREAD
  //  28    Ready to receive DMA Block  (0=No, 1=Ready)  ;GP0(...) ;via GP0

  return state.status & ~(1 << 19) |
      (1 << 13) |
      (1 << 26) |
      (1 << 27) |
      (1 << 28);
}

static inline uint32_t get_gp0_data() {
  auto data = state.gp0_fifo.front();
  state.gp0_fifo.pop_front();

  return data;
}

static inline void write_gp0(uint32_t data) {
  if (state.gp0_texture_upload_size) {
    state.gp0_texture_upload_size--;
    return;
  }

  if (state.gp0_fifo.size() == 0) {
    state.gp0_command = data >> 24;
  }

  state.gp0_fifo.push_back(data);

  if (state.gp0_fifo.size() == gp0_command_size[state.gp0_command]) {
    auto command = get_gp0_data();

    switch (state.gp0_command) {
      case 0x00: break; // nop
      case 0x01: break; // clear texture cache

      case 0x28: {
        auto vertex1 = get_gp0_data();
        auto vertex2 = get_gp0_data();
        auto vertex3 = get_gp0_data();
        auto vertex4 = get_gp0_data();
        break;
      }

      case 0x2c: { // textured quad, opaque
        auto colour  = (command & 0xffffff);
        auto vertex1 = get_gp0_data();
        auto tcoord1 = get_gp0_data();
        auto vertex2 = get_gp0_data();
        auto tcoord2 = get_gp0_data();
        auto vertex3 = get_gp0_data();
        auto tcoord3 = get_gp0_data();
        auto vertex4 = get_gp0_data();
        auto tcoord4 = get_gp0_data();
        break;
      }

      case 0x30: { // shaded triangle, opaque
        auto colour1 = (command & 0xffffff);
        auto vertex1 = get_gp0_data();
        auto colour2 = get_gp0_data();
        auto vertex2 = get_gp0_data();
        auto colour3 = get_gp0_data();
        auto vertex3 = get_gp0_data();
        break;
      }

      case 0x38: { // shaded quad, opaque
        auto colour1 = (command & 0xffffff);
        auto vertex1 = get_gp0_data();
        auto colour2 = get_gp0_data();
        auto vertex2 = get_gp0_data();
        auto colour3 = get_gp0_data();
        auto vertex3 = get_gp0_data();
        auto colour4 = get_gp0_data();
        auto vertex4 = get_gp0_data();
        break;
      }

      case 0xa0: {
        auto param1 = get_gp0_data();
        auto param2 = get_gp0_data();

        auto x = param1 & 0xffff;
        auto y = param1 >> 16;
        auto w = param2 & 0xffff;
        auto h = param2 >> 16;

        state.gp0_texture_upload_size = ((w * h) + 1) / 2;
        break;
      }

      case 0xc0: {
        auto param1 = get_gp0_data();
        auto param2 = get_gp0_data();

        auto x = param1 & 0xffff;
        auto y = param1 >> 16;
        auto w = param2 & 0xffff;
        auto h = param2 >> 16;

        state.gp0_texture_download_size = ((w * h) + 1) / 2;
        break;
      }

      case 0xe1:
        state.status &= ~0x87ff;
        state.status |= (command << 0) & 0x7ff;
        state.status |= (command << 4) & 0x8000;

        state.textured_rectangle_x_flip = ((command >> 12) & 1) != 0;
        state.textured_rectangle_y_flip = ((command >> 13) & 1) != 0;
        break;

      case 0xe2:
        state.texture_window_mask_x   = utility::uclip<5>(command >>  0);
        state.texture_window_mask_y   = utility::uclip<5>(command >>  5);
        state.texture_window_offset_x = utility::uclip<5>(command >> 10);
        state.texture_window_offset_y = utility::uclip<5>(command >> 15);
        break;

      case 0xe3:
        state.drawing_area_x1 = utility::uclip<10>(command >>  0);
        state.drawing_area_y1 = utility::uclip<10>(command >> 10);
        break;

      case 0xe4:
        state.drawing_area_x2 = utility::uclip<10>(command >>  0);
        state.drawing_area_y2 = utility::uclip<10>(command >> 10);
        break;

      case 0xe5:
        state.x_offset = utility::sclip<11>(command >>  0);
        state.y_offset = utility::sclip<11>(command >> 11);
        break;

      case 0xe6:
        state.status &= ~0x1800;
        state.status |= (command << 11) & 0x1800;
        break;

      default:
        printf("unhandled gp0 command: 0x%08x\n", command);
        break;
    }
  }
}

static inline void write_gp1(uint32_t data) {
  switch ((data >> 24) & 0xff) {
    case 0x00:
      state.status = 0x14802000;
      state.textured_rectangle_x_flip = 0;
      state.textured_rectangle_y_flip = 0;
      break;

    case 0x01: {
      state.gp0_command = 0;
      state.gp0_texture_upload_size = 0;
      state.gp0_fifo.clear();
      break;
    }

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
      state.display_area_x = utility::uclip<10>(data >>  0);
      state.display_area_y = utility::uclip< 9>(data >> 10);
      break;

    case 0x06:
      state.display_area_x1 = utility::uclip<12>(data >>  0);
      state.display_area_x2 = utility::uclip<12>(data >> 12);
      break;

    case 0x07:
      state.display_area_y1 = utility::uclip<10>(data >>  0);
      state.display_area_y2 = utility::uclip<10>(data >> 10);
      break;

    case 0x08:
      // 0-1   Horizontal Resolution 1     (0=256, 1=320, 2=512, 3=640) ;GPUSTAT.17-18
      // 2     Vertical Resolution         (0=240, 1=480, when Bit5=1)  ;GPUSTAT.19
      // 3     Video Mode                  (0=NTSC/60Hz, 1=PAL/50Hz)    ;GPUSTAT.20
      // 4     Display Area Color Depth    (0=15bit, 1=24bit)           ;GPUSTAT.21
      // 5     Vertical Interlace          (0=Off, 1=On)                ;GPUSTAT.22
      // 6     Horizontal Resolution 2     (0=256/320/512/640, 1=368)   ;GPUSTAT.16
      // 7     "Reverseflag"               (0=Normal, 1=Distorted)      ;GPUSTAT.14

      state.status &= ~0x7f4000;
      state.status |= (data << 17) & 0x7e0000;
      state.status |= (data << 10) & 0x10000;
      state.status |= (data <<  7) & 0x4000;
      break;

    default:
      printf("unhandled gp1 command: 0x%08x\n", data);
      break;
  }
}

uint32_t gpu::read(int size, uint32_t address) {
  switch (address) {
  case 0x1f801810: return read_resp();
  case 0x1f801814: return read_stat();
  }

  throw std::runtime_error("invalid gpu mmio_read");
}

void gpu::write(int size, uint32_t address, uint32_t data) {
  switch (address) {
  case 0x1f801810: return write_gp0(data);
  case 0x1f801814: return write_gp1(data);
  }

  throw std::runtime_error("invalid gpu mmio_write");
}
