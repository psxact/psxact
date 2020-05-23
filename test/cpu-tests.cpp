#include <gtest/gtest.h>
#include <cstdint>

#include "cpu/segment.hpp"

using namespace psx::cpu;

TEST(GetSegment, Works) {
  ASSERT_EQ(segment::kuseg, get_segment(0x0000'0000));
  ASSERT_EQ(segment::kuseg, get_segment(0x7fff'ffff));
  ASSERT_EQ(segment::kseg0, get_segment(0x8000'0000));
  ASSERT_EQ(segment::kseg0, get_segment(0x9fff'ffff));
  ASSERT_EQ(segment::kseg1, get_segment(0xa000'0000));
  ASSERT_EQ(segment::kseg1, get_segment(0xbfff'ffff));
  ASSERT_EQ(segment::kseg2, get_segment(0xc000'0000));
  ASSERT_EQ(segment::kseg2, get_segment(0xffff'ffff));
}
