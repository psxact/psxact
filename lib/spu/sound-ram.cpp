#include "spu/sound-ram.hpp"

using namespace psx::spu;

uint32_t sound_ram_address_t::create(uint16_t reg_value) {
  return uint32_t(reg_value) * 4;
}

uint16_t sound_ram_t::read(uint32_t address) {
  return buffer[address & sound_ram_mask];
}

void sound_ram_t::write(uint32_t address, uint16_t data) {
  buffer[address & sound_ram_mask] = data;
}
