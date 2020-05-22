#include <gtest/gtest.h>
#include "cpu/register-file.hpp"

using namespace psx::cpu;

TEST(RegisterFile, GetWorks) {
  register_file reg;

  auto read1 = reg.get(4);
  auto read2 = reg.get(4);

  ASSERT_EQ(read1, read2);
}

TEST(RegisterFile, PutWorks) {
  psx::cpu::register_file reg;

  reg.put(4, 0xcafebabe);
  ASSERT_EQ(reg.get(4), 0xcafebabe);

  reg.put(4, 0);
  ASSERT_EQ(reg.get(4), 0);
}

TEST(RegisterFile, Reg0IsAlways0) {
  psx::cpu::register_file reg;

  reg.put(0, 0xffffffff);
  ASSERT_EQ(reg.get(0), 0);
}
