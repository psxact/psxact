#include "exp/expansion1.hpp"

#include "timing.hpp"

using namespace psx::exp;

expansion1_t::expansion1_t()
  : addressable_t("exp1", false) {
}

static void add_cpu_time(psx::address_width_t width) {
  switch (width) {
    case psx::address_width_t::byte: psx::timing::add_cpu_time(8); break;
    case psx::address_width_t::half: psx::timing::add_cpu_time(14); break;
    case psx::address_width_t::word: psx::timing::add_cpu_time(26); break;
  }
}

uint32_t expansion1_t::io_read(address_width_t width, uint32_t address) {
  add_cpu_time(width);

  if (width == address_width_t::byte) {
    if (address == 0x1f000004) {
      return 0;
    }

    if (address == 0x1f000084) {
      return 0;
    }
  }

  return addressable_t::io_read(width, address);
}

void expansion1_t::io_write(address_width_t width, uint32_t address, uint32_t data) {
  add_cpu_time(width);

  return addressable_t::io_write(width, address, data);
}
