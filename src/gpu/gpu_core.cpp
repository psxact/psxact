#include "gpu_core.hpp"
#include "../utility.hpp"
#include <stdexcept>
#include <stdio.h>

static gpu::state_t state;

static int gp0_command_size[256] = {
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // $00
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // $10
  1, 1, 1, 1, 1, 1, 1, 1, 5, 1, 1, 1, 1, 1, 1, 1, // $20
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // $30
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
  //  0-3   Texture page X Base   (N*64)                              ;GP0(E1h).0-3
  //  4     Texture page Y Base   (N*256) (ie. 0 or 256)              ;GP0(E1h).4
  //  5-6   Semi Transparency     (0=B/2+F/2, 1=B+F, 2=B-F, 3=B+F/4)  ;GP0(E1h).5-6
  //  7-8   Texture page colors   (0=4bit, 1=8bit, 2=15bit, 3=Reserved)GP0(E1h).7-8
  //  9     Dither 24bit to 15bit (0=Off/strip LSBs, 1=Dither Enabled);GP0(E1h).9
  //  10    Drawing to display area (0=Prohibited, 1=Allowed)         ;GP0(E1h).10
  //  11    Set Mask-bit when drawing pixels (0=No, 1=Yes/Mask)       ;GP0(E6h).0
  //  12    Draw Pixels           (0=Always, 1=Not to Masked areas)   ;GP0(E6h).1
  //  13    "reserved"            (seems to be always set?)
  //  14    "Reverseflag"         (0=Normal, 1=Distorted)             ;GP1(08h).7
  //  15    Texture Disable       (0=Normal, 1=Disable Textures)      ;GP0(E1h).11
  //  16    Horizontal Resolution 2     (0=256/320/512/640, 1=368)    ;GP1(08h).6
  //  17-18 Horizontal Resolution 1     (0=256, 1=320, 2=512, 3=640)  ;GP1(08h).0-1
  //  19    Vertical Resolution         (0=240, 1=480, when Bit22=1)  ;GP1(08h).2
  //  20    Video Mode                  (0=NTSC/60Hz, 1=PAL/50Hz)     ;GP1(08h).3
  //  21    Display Area Color Depth    (0=15bit, 1=24bit)            ;GP1(08h).4
  //  22    Vertical Interlace          (0=Off, 1=On)                 ;GP1(08h).5
  //  23    Display Enable              (0=Enabled, 1=Disabled)       ;GP1(03h).0
  //  24    Interrupt Request (IRQ1)    (0=Off, 1=IRQ)       ;GP0(1Fh)/GP1(02h)
  //  25    DMA / Data Request, meaning depends on GP1(04h) DMA Direction:
  //  When GP1(04h)=0 ---> Always zero (0)
  //  When GP1(04h)=1 ---> FIFO State  (0=Full, 1=Not Full)
  //  When GP1(04h)=2 ---> Same as GPUSTAT.28
  //  When GP1(04h)=3 ---> Same as GPUSTAT.27
  //  26    Ready to receive Cmd Word   (0=No, 1=Ready)  ;GP0(...) ;via GP0
  //  27    Ready to send VRAM to CPU   (0=No, 1=Ready)  ;GP0(C0h) ;via GPUREAD
  //  28    Ready to receive DMA Block  (0=No, 1=Ready)  ;GP0(...) ;via GP0
  //  29-30 DMA Direction (0=Off, 1=?, 2=CPUtoGP0, 3=GPUREADtoCPU)    ;GP1(04h).0-1
  //  31    Drawing even/odd lines in interlace mode (0=Even or Vblank, 1=Odd)

  return state.status & ~(1 << 19) |
      (1 << 13) |
      (1 << 26) |
      (1 << 27) |
      (1 << 28);
}

static inline uint32_t get_gp0_data() {
  auto param = state.gp0_fifo.front();
  state.gp0_fifo.pop();

  return param;
}

static inline void write_gp0(uint32_t data) {
  if (state.gp0_texture_upload_size) {
    state.gp0_texture_upload_size--;
    return;
  }

  if (state.gp0_fifo.size() == 0) {
    state.gp0_command = data >> 24;
  }

  state.gp0_fifo.push(data);

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

        // printf("gpu::draw_monochrome_quad()\n");
        // printf("  command: $%08x\n", command);
        // printf("  vertex1: (%03d, %03d)\n", vertex1 & 0xffff, vertex1 >> 16);
        // printf("  vertex2: (%03d, %03d)\n", vertex2 & 0xffff, vertex2 >> 16);
        // printf("  vertex3: (%03d, %03d)\n", vertex3 & 0xffff, vertex3 >> 16);
        // printf("  vertex4: (%03d, %03d)\n", vertex4 & 0xffff, vertex4 >> 16);
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

      std::queue<uint32_t> empty;
      state.gp0_fifo.swap(empty);
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
