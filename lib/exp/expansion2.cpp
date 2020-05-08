#include "exp/expansion2.hpp"

#include <cstdio>

using namespace psx::exp;

expansion2_t::expansion2_t()
  : addressable_t("exp2", false) {
}

uint32_t expansion2_t::io_read(address_width_t width, uint32_t address) {
  if (width == address_width_t::byte) {
    switch (address) {
      case 0x1f802021: return 0x04;
    }
  }

  return addressable_t::io_read(width, address);
}

void expansion2_t::io_write(address_width_t width, uint32_t address, uint32_t data) {
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
