#include "exp/expansion1.hpp"

using namespace psx::exp;

expansion1_t::expansion1_t()
  : addressable_t("exp1", false) {
}

uint8_t expansion1_t::io_read_byte(uint32_t address) {
  if (address == 0x1f000004) {
    return 0;
  }

  if (address == 0x1f000084) {
    return 0;
  }

  return addressable_t::io_read_byte(address);
}
