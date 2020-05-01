#include <gtest/gtest.h>
#include "gpu/core.hpp"

using namespace psx;
using namespace psx::gpu;

class MockInterruptible : public interruptible_t {
  void interrupt(interrupt_type_t) override {
  }
};

class MockIrqLine : public irq_line_t {
public:
  MockIrqLine()
    : irq_line_t(interruptible, interrupt_type_t::gpu) {
  }

  MockInterruptible interruptible;
};

class GpuTest : public ::testing::Test {
public:
  GpuTest()
    : gpu(irq) {
  }

  MockIrqLine irq;
  core_t gpu;
};

TEST_F(GpuTest, Gp1_08_DisplayMode) {
  // Test initial values

  EXPECT_EQ(gpu.get_h_resolution(), gpu_h_resolution_t::h256);
  EXPECT_EQ(gpu.get_v_resolution(), gpu_v_resolution_t::v240);

  // Test setting horizontal resolution, without the "Force 368" bit.

  gpu.gp1(0x08000000);
  EXPECT_EQ(gpu.get_h_resolution(), gpu_h_resolution_t::h256);

  gpu.gp1(0x08000001);
  EXPECT_EQ(gpu.get_h_resolution(), gpu_h_resolution_t::h320);

  gpu.gp1(0x08000002);
  EXPECT_EQ(gpu.get_h_resolution(), gpu_h_resolution_t::h512);

  gpu.gp1(0x08000003);
  EXPECT_EQ(gpu.get_h_resolution(), gpu_h_resolution_t::h640);

  // Test setting horizontal resolution, with the "Force 368" bit.

  gpu.gp1(0x08000040);
  EXPECT_EQ(gpu.get_h_resolution(), gpu_h_resolution_t::h368);

  gpu.gp1(0x08000041);
  EXPECT_EQ(gpu.get_h_resolution(), gpu_h_resolution_t::h368);

  gpu.gp1(0x08000042);
  EXPECT_EQ(gpu.get_h_resolution(), gpu_h_resolution_t::h368);

  gpu.gp1(0x08000043);
  EXPECT_EQ(gpu.get_h_resolution(), gpu_h_resolution_t::h368);

  // Test setting vertical resolution, without the "Interlace" bit.

  gpu.gp1(0x08000000);
  EXPECT_EQ(gpu.get_v_resolution(), gpu_v_resolution_t::v240);

  gpu.gp1(0x08000004);
  EXPECT_EQ(gpu.get_v_resolution(), gpu_v_resolution_t::v240);

  // Test setting vertical resolution, with the "Interlace" bit.

  gpu.gp1(0x08000020);
  EXPECT_EQ(gpu.get_v_resolution(), gpu_v_resolution_t::v240);

  gpu.gp1(0x08000024);
  EXPECT_EQ(gpu.get_v_resolution(), gpu_v_resolution_t::v480);

  // Test setting display depth.

  gpu.gp1(0x08000000);
  EXPECT_EQ(gpu.get_display_depth(), gpu_display_depth_t::bpp15);

  gpu.gp1(0x08000010);
  EXPECT_EQ(gpu.get_display_depth(), gpu_display_depth_t::bpp24);
}

TEST_F(GpuTest, StatusBits) {
  gpu.gp1(0); // Reset
  ASSERT_EQ(gpu.stat(), 0x14802000);

  // 0-3   Texture page X Base   (N*64)                              ;GP0(E1h).0-3
  gpu.gp0(0xE100'000F);
  ASSERT_EQ(gpu.stat() & 0xF, 0xF);

  // 4     Texture page Y Base   (N*256) (ie. 0 or 256)              ;GP0(E1h).4
  gpu.gp0(0xE100'0010);
  ASSERT_EQ(gpu.stat() & 0x10, 0x10);

  // 5-6   Semi Transparency     (0=B/2+F/2, 1=B+F, 2=B-F, 3=B+F/4)  ;GP0(E1h).5-6
  gpu.gp0(0xE100'0060);
  ASSERT_EQ(gpu.stat() & 0x60, 0x60);

  // 7-8   Texture page colors   (0=4bit, 1=8bit, 2=15bit, 3=Reserved)GP0(E1h).7-8
  gpu.gp0(0xE100'0180);
  ASSERT_EQ(gpu.stat() & 0x180, 0x180);

  // 9     Dither 24bit to 15bit (0=Off/strip LSBs, 1=Dither Enabled);GP0(E1h).9
  gpu.gp0(0xE100'0200);
  ASSERT_EQ(gpu.stat() & 0x200, 0x200);

  // 10    Drawing to display area (0=Prohibited, 1=Allowed)         ;GP0(E1h).10
  gpu.gp0(0xE100'0400);
  ASSERT_EQ(gpu.stat() & 0x400, 0x400);

  // 11    Set Mask-bit when drawing pixels (0=No, 1=Yes/Mask)       ;GP0(E6h).0
  gpu.gp0(0xE600'0001);
  ASSERT_EQ(gpu.stat() & 0x800, 0x800);

  // 12    Draw Pixels           (0=Always, 1=Not to Masked areas)   ;GP0(E6h).1
  gpu.gp0(0xE600'0002);
  ASSERT_EQ(gpu.stat() & 0x1000, 0x1000);

  // 13    Interlace Field       (or, always 1 when GP1(08h).5=0)
  // 14    "Reverseflag"         (0=Normal, 1=Distorted)             ;GP1(08h).7
  gpu.gp1(0x08000080);
  ASSERT_EQ(gpu.stat() & 0x4000, 0x4000);

  // 15    Texture Disable       (0=Normal, 1=Disable Textures)      ;GP0(E1h).11
  gpu.gp0(0xE1000800);
  ASSERT_EQ(gpu.stat() & 0x8000, 0x8000);

  // 16    Horizontal Resolution 2     (0=256/320/512/640, 1=368)    ;GP1(08h).6
  gpu.gp1(0x08000040);
  ASSERT_EQ(gpu.stat() & 0x10000, 0x10000);

  // 17-18 Horizontal Resolution 1     (0=256, 1=320, 2=512, 3=640)  ;GP1(08h).0-1
  gpu.gp1(0x08000003);
  ASSERT_EQ(gpu.stat() & 0x60000, 0x60000);

  // 19    Vertical Resolution         (0=240, 1=480, when Bit22=1)  ;GP1(08h).2
  gpu.gp1(0x08000004);
  ASSERT_EQ(gpu.stat() & 0x80000, 0x80000);

  // 20    Video Mode                  (0=NTSC/60Hz, 1=PAL/50Hz)     ;GP1(08h).3
  gpu.gp1(0x08000008);
  ASSERT_EQ(gpu.stat() & 0x100000, 0x100000);

  // 21    Display Area Color Depth    (0=15bit, 1=24bit)            ;GP1(08h).4
  gpu.gp1(0x08000010);
  ASSERT_EQ(gpu.stat() & 0x200000, 0x200000);

  // 22    Vertical Interlace          (0=Off, 1=On)                 ;GP1(08h).5
  gpu.gp1(0x08000020);
  ASSERT_EQ(gpu.stat() & 0x400000, 0x400000);

  // 23    Display Enable              (0=Enabled, 1=Disabled)       ;GP1(03h).0
  gpu.gp1(0x03000001);
  ASSERT_EQ(gpu.stat() & 0x800000, 0x800000);

  // 24    Interrupt Request (IRQ1)    (0=Off, 1=IRQ)       ;GP0(1Fh)/GP1(02h)
  gpu.gp0(0x1F000000);
  ASSERT_EQ(gpu.stat() & 0x1000000, 0x1000000);

  gpu.gp1(0x02000000);
  ASSERT_EQ(gpu.stat() & 0x1000000, 0);

  // 29-30 DMA Direction (0=Off, 1=?, 2=CPUtoGP0, 3=GPUREADtoCPU)    ;GP1(04h).0-1
  gpu.gp1(0x04000003);
  ASSERT_EQ(gpu.stat() & 0x60000000, 0x60000000);
  
  // 31    Drawing even/odd lines in interlace mode (0=Even or Vblank, 1=Odd)
}
