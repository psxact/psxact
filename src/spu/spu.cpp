#include "spu.hpp"
#include "../utility.hpp"


uint32_t spu_t::io_read(bus_width_t width, uint32_t address) {
  if (address >= 0x1f801c00 && address <= 0x1f801d7f) {
    auto n = (address >> 4) & 31;
    auto m = (address >> 1) & 7;

    return registers[n][m];
  }

  switch (address) {
  case 0x1f801da6:
    return sound_ram_address_latch;

  case 0x1f801daa:
    return control;

  case 0x1f801dac:
    return sound_ram_transfer_control;

  case 0x1f801dae:
    return control & 0x3f;
  }

  if (utility::log_spu) {
    printf("spu::io_read(%d, 0x%08x)\n", width, address);
  }

  return 0;
}


void spu_t::io_write(bus_width_t width, uint32_t address, uint32_t data) {
  if (address >= 0x1f801c00 && address <= 0x1f801d7f) {
    auto n = (address >> 4) & 31;
    auto m = (address >> 1) & 7;

    registers[n][m] = uint16_t(data);
    return;
  }

  switch (address) {
  case 0x1f801da6:
    sound_ram_address = uint16_t(data) << 3;
    sound_ram_address_latch = uint16_t(data);
    return;

  case 0x1f801da8:
    sound_ram.write_half(sound_ram_address, data);
    sound_ram_address = (sound_ram_address + 1) & 0x7ffff;
    return;

  case 0x1f801daa:
    control = uint16_t(data);
    return;
  }

  if (utility::log_spu) {
    printf("spu::io_write(%d, 0x%08x, 0x%08x)\n", width, address, data);
  }
}
