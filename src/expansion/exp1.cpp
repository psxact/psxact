#include "expansion/exp1.hpp"


using namespace psx;

exp1_t::exp1_t()
  : memory_component_t("exp1") {
}


uint32_t exp1_t::io_read_byte(uint32_t address) {
  if (address == 0x1f000004) {
    return 0;
  }

  if (address == 0x1f000084) {
    return 0;
  }

  return memory_component_t::io_read_byte(address);
}
