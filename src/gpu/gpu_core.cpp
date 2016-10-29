#include "gpu_core.hpp"
#include <stdexcept>
#include <stdio.h>

static gpu::state_t state;

static inline uint32_t read_resp() {
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

  printf("gpu::read_stat()\n");

  return state.status |
      (1 << 26) |
      (1 << 27) |
      (1 << 28);
}

static inline void write_gp0(uint32_t command) {
  switch ((command >> 24) & 0xff) {
    case 0x00: /* nop */ break;
    case 0xe1:
      // 0-3   Texture page X Base   (N*64) (ie. in 64-halfword steps)    ;GPUSTAT.0-3
      // 4     Texture page Y Base   (N*256) (ie. 0 or 256)               ;GPUSTAT.4
      // 5-6   Semi Transparency     (0=B/2+F/2, 1=B+F, 2=B-F, 3=B+F/4)   ;GPUSTAT.5-6
      // 7-8   Texture page colors   (0=4bit, 1=8bit, 2=15bit, 3=Reserved);GPUSTAT.7-8
      // 9     Dither 24bit to 15bit (0=Off/strip LSBs, 1=Dither Enabled) ;GPUSTAT.9
      // 10    Drawing to display area (0=Prohibited, 1=Allowed)          ;GPUSTAT.10
      // 11    Texture Disable (0=Normal, 1=Disable if GP1(09h).Bit0=1)   ;GPUSTAT.15
      state.status &= ~0x87ff;
      state.status |= (command << 0) & 0x7ff;
      state.status |= (command << 4) & 0x8000;

      // 12    Textured Rectangle X-Flip   (BIOS does set this bit on power-up...?)
      // 13    Textured Rectangle Y-Flip   (BIOS does set it equal to GPUSTAT.13...?)
      state.textured_rectangle_x_flip = (command >> 12) & 1;
      state.textured_rectangle_y_flip = (command >> 13) & 1;
      break;

    default:
      printf("unhandled gp0 command: 0x%08x\n", command);
      break;
  }
}

static inline void write_gp1(uint32_t command) {
  switch ((command >> 24) & 0xff) {
    case 0x00:
      state.status = 0x14802000;
      state.textured_rectangle_x_flip = 0;
      state.textured_rectangle_y_flip = 0;
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
      state.status |= (command << 17) & 0x7e0000;
      state.status |= (command << 10) & 0x10000;
      state.status |= (command <<  7) & 0x4000;
      break;

    default:
      printf("unhandled gp1 command: 0x%08x\n", command);
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
