#include <gtest/gtest.h>
#include "timer/core.hpp"

using namespace psx;
using namespace psx::timer;
using namespace psx::util;

class TimerTest : public ::testing::Test {
  public:
    TimerTest()
      : timer(wire(), wire(), wire()) {
    }

  protected:
    psx::timer::core timer;
};

TEST_F(TimerTest, Timer0_Initialized) {
  ASSERT_EQ(0, timer.io_read(address_width::half, 0x1F801100));
  ASSERT_EQ(0, timer.io_read(address_width::half, 0x1F801104));
  ASSERT_EQ(0, timer.io_read(address_width::half, 0x1F801108));
}

TEST_F(TimerTest, Timer1_Initialized) {
  ASSERT_EQ(0, timer.io_read(address_width::half, 0x1F801110));
  ASSERT_EQ(0, timer.io_read(address_width::half, 0x1F801114));
  ASSERT_EQ(0, timer.io_read(address_width::half, 0x1F801118));
}

TEST_F(TimerTest, Timer2_Initialized) {
  ASSERT_EQ(0, timer.io_read(address_width::half, 0x1F801120));
  ASSERT_EQ(0, timer.io_read(address_width::half, 0x1F801124));
  ASSERT_EQ(0, timer.io_read(address_width::half, 0x1F801128));
}

TEST_F(TimerTest, Timer2_SystemBasics) {
  timer.io_write(address_width::half, 0x1F801120, 0);
  timer.io_write(address_width::half, 0x1F801124, 0);
  timer.io_write(address_width::half, 0x1F801128, 0);
  timer.tick(8);

  ASSERT_EQ(8, timer.io_read(address_width::half, 0x1F801120));
}

TEST_F(TimerTest, Timer2_SystemLargeValues) {
  timer.io_write(address_width::half, 0x1F801120, 0);
  timer.io_write(address_width::half, 0x1F801124, 0);
  timer.io_write(address_width::half, 0x1F801128, 0);
  timer.tick(65535);

  ASSERT_EQ(65535, timer.io_read(address_width::half, 0x1F801120));
}

TEST_F(TimerTest, Timer2_SystemOver8Basics) {
  timer.io_write(address_width::half, 0x1F801120, 0);
  timer.io_write(address_width::half, 0x1F801124, 0x200);
  timer.io_write(address_width::half, 0x1F801128, 0);

  timer.tick(8);
  ASSERT_EQ(1, timer.io_read(address_width::half, 0x1F801120));

  timer.tick(7);
  ASSERT_EQ(1, timer.io_read(address_width::half, 0x1F801120));

  timer.tick(1);
  ASSERT_EQ(2, timer.io_read(address_width::half, 0x1F801120));
}

TEST_F(TimerTest, Timer2_SystemOver8LargeValues) {
  timer.io_write(address_width::half, 0x1F801120, 0);
  timer.io_write(address_width::half, 0x1F801124, 0x200);
  timer.io_write(address_width::half, 0x1F801128, 0);

  timer.tick(524'280);
  ASSERT_EQ(0xFFFF, timer.io_read(address_width::half, 0x1F801120));

  timer.tick(7); // Test that remainders carry over
  ASSERT_EQ(0xFFFF, timer.io_read(address_width::half, 0x1F801120));

  timer.tick(1);
  ASSERT_EQ(0x0000, timer.io_read(address_width::half, 0x1F801120));
}
