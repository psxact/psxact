#include "cpu/register-file.hpp"

using namespace psx::cpu;

uint32_t register_file_t::get(uint32_t reg) const {
  return registers[reg];
}

void register_file_t::put(uint32_t reg, uint32_t val) {
  if (reg) {
    registers[reg] = val;
  } else {
    registers[reg] = 0;
  }
}
