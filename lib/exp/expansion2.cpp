#include "exp/expansion2.hpp"

#include <cstdio>
#include "timing.hpp"

using namespace psx::exp;

expansion2_t::expansion2_t()
  : addressable_t("exp2", false) {
}

static void add_cpu_time(psx::address_width_t width) {
  switch (width) {
    case psx::address_width_t::byte: psx::timing::add_cpu_time(12); break;
    case psx::address_width_t::half: psx::timing::add_cpu_time(27); break;
    case psx::address_width_t::word: psx::timing::add_cpu_time(57); break;
  }
}

uint32_t expansion2_t::io_read(address_width_t width, uint32_t address) {
  add_cpu_time(width);

  if (width == address_width_t::byte) {
    switch (address) {
      case 0x1f802021: return 0x04;
    }
  }

  return addressable_t::io_read(width, address);
}

void expansion2_t::io_write(address_width_t width, uint32_t address, uint32_t data) {
  add_cpu_time(width);

  if (width == address_width_t::byte) {
    switch (address) {
      case 0x1f802020: return;
      case 0x1f802021: return;
      case 0x1f802022: return;
      case 0x1f802023: putc(data, stdout); return;
      case 0x1f802024: return;
      case 0x1f802025: return;
      case 0x1f80202a: return;
      case 0x1f80202d: return;
      case 0x1f80202e: return;
      case 0x1f802041: return;
    }
  }

  return addressable_t::io_write(width, address, data);
}
