#include <gtest/gtest.h>
#include "util/wire.hpp"

using namespace psx::util;

TEST(Wire, LowLevelSensitivityWorks) {
  int cb_happened = 0;

  wire wire;
  wire.recv_off([&]() { cb_happened++; });
  wire(wire_state::off);

  ASSERT_EQ(cb_happened, 1);

  wire(wire_state::on);

  ASSERT_EQ(cb_happened, 1);

  wire(wire_state::on);

  ASSERT_EQ(cb_happened, 1);

  wire(wire_state::off);

  ASSERT_EQ(cb_happened, 2);
}

TEST(Wire, HighLevelSensitivityWorks) {
  int cb_happened = 0;

  wire wire;
  wire.recv_on([&]() { cb_happened++; });
  wire(wire_state::off);

  ASSERT_EQ(cb_happened, 0);

  wire(wire_state::on);

  ASSERT_EQ(cb_happened, 1);

  wire(wire_state::on);

  ASSERT_EQ(cb_happened, 2);

  wire(wire_state::off);

  ASSERT_EQ(cb_happened, 2);
}

TEST(Wire, FallingEdgeSensitivityWorks) {
  int cb_happened = 0;

  wire wire;
  wire.recv_fall([&]() { cb_happened++; });
  wire(wire_state::off);

  ASSERT_EQ(cb_happened, 0);

  wire(wire_state::on);

  ASSERT_EQ(cb_happened, 0);

  wire(wire_state::on);

  ASSERT_EQ(cb_happened, 0);

  wire(wire_state::off);

  ASSERT_EQ(cb_happened, 1);
}

TEST(Wire, RisingEdgeSensitivityWorks) {
  int cb_happened = 0;

  wire wire;
  wire.recv_rise([&]() { cb_happened++; });
  wire(wire_state::off);

  ASSERT_EQ(cb_happened, 0);

  wire(wire_state::on);

  ASSERT_EQ(cb_happened, 1);

  wire(wire_state::on);

  ASSERT_EQ(cb_happened, 1);

  wire(wire_state::off);

  ASSERT_EQ(cb_happened, 1);
}
