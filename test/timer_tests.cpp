#include <gtest/gtest.h>
#include <timer/core.hpp>

TEST(Timer0, Initialized) {
  psx::timer::core_t timer(nullptr, false);

  ASSERT_EQ(0, timer.io_read_word(0x1F801100));
  ASSERT_EQ(0, timer.io_read_word(0x1F801104));
  ASSERT_EQ(0, timer.io_read_word(0x1F801108));
}

TEST(Timer1, Initialized) {
  psx::timer::core_t timer(nullptr, false);

  ASSERT_EQ(0, timer.io_read_word(0x1F801110));
  ASSERT_EQ(0, timer.io_read_word(0x1F801114));
  ASSERT_EQ(0, timer.io_read_word(0x1F801118));
}

TEST(Timer2, Initialized) {
  psx::timer::core_t timer(nullptr, false);

  ASSERT_EQ(0, timer.io_read_word(0x1F801120));
  ASSERT_EQ(0, timer.io_read_word(0x1F801124));
  ASSERT_EQ(0, timer.io_read_word(0x1F801128));
}

TEST(Timer2, SystemBasics) {
  psx::timer::core_t timer(nullptr, false);

  timer.io_write_half(0x1F801120, 0);
  timer.io_write_half(0x1F801124, 0);
  timer.io_write_half(0x1F801128, 0);
  timer.run(8);

  ASSERT_EQ(8, timer.io_read_word(0x1F801120));
}

TEST(Timer2, SystemLargeValues) {
  psx::timer::core_t timer(nullptr, false);

  timer.io_write_half(0x1F801120, 0);
  timer.io_write_half(0x1F801124, 0);
  timer.io_write_half(0x1F801128, 0);
  timer.run(65535);

  ASSERT_EQ(65535, timer.io_read_word(0x1F801120));
}

TEST(Timer2, SystemOver8Basics) {
  psx::timer::core_t timer(nullptr, false);

  timer.io_write_half(0x1F801120, 0);
  timer.io_write_half(0x1F801124, 0x200);
  timer.io_write_half(0x1F801128, 0);
  timer.run(8);

  ASSERT_EQ(1, timer.io_read_word(0x1F801120));
}

TEST(Timer2, SystemOver8LargeValues) {
  psx::timer::core_t timer(nullptr, false);

  timer.io_write_half(0x1F801120, 0);
  timer.io_write_half(0x1F801124, 0x200);
  timer.io_write_half(0x1F801128, 0);
  timer.run(524'280);

  ASSERT_EQ(0xffff, timer.io_read_word(0x1F801120));

  timer.run(7); // Test that remainders carry over

  ASSERT_EQ(0xffff, timer.io_read_word(0x1F801120));

  timer.run(1);

  ASSERT_EQ(0x0000, timer.io_read_word(0x1F801120));
}
