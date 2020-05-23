#include "exp/expansion1.hpp"

#include "timing.hpp"

using namespace psx::exp;

expansion1::expansion1()
  : addressable("exp1", false) {
}

static void add_cpu_time(psx::address_width width) {
  switch (width) {
    case psx::address_width::byte: psx::timing::add_cpu_time(8); break;
    case psx::address_width::half: psx::timing::add_cpu_time(14); break;
    case psx::address_width::word: psx::timing::add_cpu_time(26); break;
  }
}

uint32_t expansion1::io_read(address_width width, uint32_t address) {
  add_cpu_time(width);

  if (width == address_width::byte) {
    if (address == 0x1f000004) {
      return 0;
    }

    if (address == 0x1f000084) {
      return 0;
    }
  }

  return addressable::io_read(width, address);
}

void expansion1::io_write(address_width width, uint32_t address, uint32_t data) {
  add_cpu_time(width);

  return addressable::io_write(width, address, data);
}
