#include "exp/expansion2.hpp"

#include <cstdio>

using namespace psx::exp;

expansion2_t::expansion2_t()
  : memory_component_t("exp2", false) {
}

uint8_t expansion2_t::io_read_byte(uint32_t address) {
  switch (address) {
    case 0x1f802021: return 0x04;
  }

  return memory_component_t::io_read_byte(address);
}

void expansion2_t::io_write_byte(uint32_t address, uint8_t data) {
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

  return memory_component_t::io_write_byte(address, data);
}
