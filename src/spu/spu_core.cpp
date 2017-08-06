#include "spu_core.hpp"
#include "../utility.hpp"

uint32_t spu::io_read(spu_state_t &state, bus_width_t width, uint32_t address) {
  if (address >= 0x1f801c00 && address <= 0x1f801d7f) {
    auto n = (address >> 4) & 31;
    auto m = (address >> 1) & 7;

    return state.registers[n][m];
  }

  switch (address) {
  case 0x1f801da6:
    return state.sound_ram_address_latch;

  case 0x1f801daa:
    return state.control;

  case 0x1f801dac:
    return state.sound_ram_transfer_control;

  case 0x1f801dae:
    return state.control & 0x3f;
  }

  if (utility::log_spu) {
    printf("spu::io_read(%d, 0x%08x)\n", width, address);
  }

  return 0;
}

void spu::io_write(spu_state_t &state, bus_width_t width, uint32_t address, uint32_t data) {
  if (address >= 0x1f801c00 && address <= 0x1f801d7f) {
    auto n = (address >> 4) & 31;
    auto m = (address >> 1) & 7;

    state.registers[n][m] = uint16_t(data);
    return;
  }

  switch (address) {
  case 0x1f801da6:
    state.sound_ram_address = uint16_t(data) << 3;
    state.sound_ram_address_latch = uint16_t(data);
    return;

  case 0x1f801da8:
    state.sound_ram.write_half(state.sound_ram_address, data);
    state.sound_ram_address = (state.sound_ram_address + 1) & 0x7ffff;
    return;

  case 0x1f801daa:
    state.control = uint16_t(data);
    return;
  }

  if (utility::log_spu) {
    printf("spu::io_write(%d, 0x%08x, 0x%08x)\n", width, address, data);
  }
}
