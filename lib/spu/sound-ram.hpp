#ifndef SPU_SOUND_RAM_HPP_
#define SPU_SOUND_RAM_HPP_

#include <cstdint>

namespace psx::spu {

  struct sound_ram_address final {
    static uint32_t create(uint16_t reg_value);
  };

  constexpr int sound_ram_size = 262144;
  constexpr int sound_ram_mask = sound_ram_size - 1;

  class sound_ram final {
    uint16_t buffer[sound_ram_size];

  public:
    uint16_t read(uint32_t address);
    void write(uint32_t address, uint16_t data);
  };
}

#endif
