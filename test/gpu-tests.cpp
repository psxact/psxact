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
    : gpu(irq, false) {
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
}
