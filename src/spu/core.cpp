#include "spu/core.hpp"

#include <cassert>
#include "util/int.hpp"
#include "util/uint.hpp"

using namespace psx::spu;
using namespace psx::util;

core_t::core_t(bool log_enabled)
  : addressable_t("spu", log_enabled)
  , sound_ram("sound-ram") {
}

core_t::~core_t() {
}

uint16_t core_t::io_read_half(uint32_t address) {
  log("io_read_half(0x%08x)", address);

  if (address >= 0x1f801c00 && address <= 0x1f801d7f) {
    auto n = (address >> 4) & 31;
    auto m = (address >> 1) & 7;

    return registers[n][m];
  }

  switch (address) {
    case 0x1f801d80:
      return (main_volume_left & 0xffff);

    case 0x1f801d82:
      return (main_volume_right & 0xffff);

    case 0x1f801d88:
      return uint_t<16>::trunc(key_on);

    case 0x1f801d8a:
      return uint_t<8>::trunc(key_on >> 16);

    case 0x1f801d8c:
      return uint_t<16>::trunc(key_off);

    case 0x1f801d8e:
      return uint_t<8>::trunc(key_off >> 16);

    case 0x1f801d90:
      return uint_t<16>::trunc(pitch_modulation_on);

    case 0x1f801d92:
      return uint_t<8>::trunc(pitch_modulation_on >> 16);

    case 0x1f801d94:
      return uint_t<16>::trunc(noise_on);

    case 0x1f801d96:
      return uint_t<8>::trunc(noise_on >> 16);

    case 0x1f801d98:
      return uint_t<16>::trunc(echo_on);

    case 0x1f801d9a:
      return uint_t<8>::trunc(echo_on >> 16);

    case 0x1f801d9c:
      return uint_t<16>::trunc(voice_status);

    case 0x1f801d9e:
      return uint_t<8>::trunc(voice_status >> 16);

    case 0x1f801da6:
      return sound_ram_address_latch;

    case 0x1f801daa:
      return control;

    case 0x1f801dac:
      return sound_ram_transfer_control;

    case 0x1f801dae:
      return (control & 0x3f);

    case 0x1f801db0:
      return uint_t<16>::trunc(cd_input_volume_left);

    case 0x1f801db2:
      return uint_t<16>::trunc(cd_input_volume_right);

    case 0x1f801db8:
      return uint_t<16>::trunc(current_main_volume_left);

    case 0x1f801dba:
      return uint_t<16>::trunc(current_main_volume_right);
  }

  return addressable_t::io_read_half(address);
}

void core_t::io_write_half(uint32_t address, uint16_t data) {
  log("io_write_half(0x%08x, 0x%04x)", address, data);

  if (address >= 0x1f801c00 && address <= 0x1f801d7f) {
    auto n = (address >> 4) & 31;
    auto m = (address >> 1) & 7;

    registers[n][m] = uint16_t(data);
    return;
  }

  if (address >= 0x1f801dc0 && address <= 0x1f801dff) {
    auto n = (address >> 1) & 0x1f;

    reverb.registers[n] = int_t<16>::trunc(data);
    return;
  }

  switch (address) {
    case 0x1f801d80:
      current_main_volume_left = int_t<16>::trunc(data);
      return;

    case 0x1f801d82:
      current_main_volume_right = int_t<16>::trunc(data);
      return;

    case 0x1f801d84:
      reverb.output_volume_left = int_t<16>::trunc(data);
      return;

    case 0x1f801d86:
      reverb.output_volume_right = int_t<16>::trunc(data);
      return;

    case 0x1f801d88:
      key_on &= 0xff0000;
      key_on |= uint_t<16>::trunc(data);
      return;

    case 0x1f801d8a:
      key_on &= 0x00ffff;
      key_on |= uint_t<8>::trunc(data) << 16;
      return;

    case 0x1f801d8c:
      key_off &= 0xff0000;
      key_off |= uint_t<16>::trunc(data);
      return;

    case 0x1f801d8e:
      key_off &= 0x00ffff;
      key_off |= uint_t<8>::trunc(data) << 16;
      return;

    case 0x1f801d90:
      pitch_modulation_on &= 0xff0000;
      pitch_modulation_on |= uint_t<16>::trunc(data);
      return;

    case 0x1f801d92:
      pitch_modulation_on &= 0x00ffff;
      pitch_modulation_on |= uint_t<8>::trunc(data) << 16;
      return;

    case 0x1f801d94:
      noise_on &= 0xff0000;
      noise_on |= uint_t<16>::trunc(data);
      return;

    case 0x1f801d96:
      noise_on &= 0x00ffff;
      noise_on |= uint_t<8>::trunc(data) << 16;
      return;

    case 0x1f801d98:
      echo_on &= 0xff0000;
      echo_on |= uint_t<16>::trunc(data);
      return;

    case 0x1f801d9a:
      echo_on &= 0x00ffff;
      echo_on |= uint_t<8>::trunc(data) << 16;
      return;

    case 0x1f801d9c: return;
    case 0x1f801d9e: return;

    case 0x1f801da2:
      reverb.start_address = uint_t<16>::trunc(data);
      return;

    case 0x1f801da6:
      sound_ram_address = uint16_t(data) << 3;
      sound_ram_address_latch = uint16_t(data);
      return;

    case 0x1f801da8:
      sound_ram.io_write_half(sound_ram_address, data);
      sound_ram_address = (sound_ram_address + 1) & 0x7ffff;
      return;

    case 0x1f801daa:
      control = uint16_t(data);
      return;

    case 0x1f801dac:
      assert(uint_t<16>::trunc(data) == 0x0004);
      return;

    case 0x1f801db0:
      cd_input_volume_left = int_t<16>::trunc(data);
      return;

    case 0x1f801db2:
      cd_input_volume_right = int_t<16>::trunc(data);
      return;

    case 0x1f801db4:
      external_input_volume_left = int_t<16>::trunc(data);
      return;

    case 0x1f801db6:
      external_input_volume_right = int_t<16>::trunc(data);
      return;
  }

  return addressable_t::io_write_half(address, data);
}

void core_t::io_write_word(uint32_t address, uint32_t data) {
  switch (address) {
    case 0x1f801da8:
      io_write_half(address, data);
      io_write_half(address, data >> 16);
      return;
  }

  addressable_t::io_write_word(address, data);
}
