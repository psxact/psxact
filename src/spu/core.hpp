// Copyright 2018 psxact

#ifndef SPU_CORE_HPP_
#define SPU_CORE_HPP_


#include "console.hpp"
#include "memory.hpp"
#include "memory-component.hpp"


namespace psx {
namespace spu {

class core_t : public memory_component_t {
  uint16_t control;
  uint16_t status;

  uint16_t registers[24][8];

  memory_t< kib(512) > sound_ram;

  uint32_t sound_ram_address;
  uint16_t sound_ram_address_latch;
  uint16_t sound_ram_transfer_control;

  int16_t cd_input_volume_left;
  int16_t cd_input_volume_right;
  int16_t external_input_volume_left;
  int16_t external_input_volume_right;
  int16_t current_main_volume_left;
  int16_t current_main_volume_right;
  int16_t main_volume_left;
  int16_t main_volume_right;
  int32_t echo_on;
  int32_t key_on;
  int32_t key_off;
  int32_t noise_on;
  int32_t pitch_modulation_on;
  int32_t voice_status;

  struct {
    uint16_t start_address;
    int16_t output_volume_left;
    int16_t output_volume_right;
    int16_t registers[32];
  } reverb;

 public:
  core_t();

  uint32_t io_read_half(uint32_t address);

  void io_write_half(uint32_t address, uint32_t data);
  void io_write_word(uint32_t address, uint32_t data);
};

}  // namespace spu
}  // namespace psx

#endif  // SPU_CORE_HPP_
