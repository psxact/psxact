#include "expansion/exp2.hpp"


exp2_t::exp2_t()
  : memory_component_t("exp2") {
}


void exp2_t::io_write_byte(uint32_t address, uint32_t data) {
  if (address == 0x1f802041) {
    return;
  }

  return memory_component_t::io_write_byte(address, data);
}
