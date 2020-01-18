#include "exp/expansion2.hpp"

using namespace psx::exp;

expansion2_t::expansion2_t()
  : memory_component_t("exp2", false) {
}

void expansion2_t::io_write_byte(uint32_t address, uint8_t data) {
  if (address == 0x1f802041) {
    return;
  }

  return memory_component_t::io_write_byte(address, data);
}
