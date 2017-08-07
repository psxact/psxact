#ifndef __PSXACT_SPU_CORE_HPP__
#define __PSXACT_SPU_CORE_HPP__

#include "../system_core.hpp"
#include "../memory.hpp"

namespace psxact {
  struct spu_core {
    uint16_t control;
    uint16_t status;

    uint16_t registers[24][8];

    memory_t<19> sound_ram;

    uint32_t sound_ram_address;
    uint16_t sound_ram_address_latch;
    uint16_t sound_ram_transfer_control;

    uint32_t io_read(bus_width_t width, uint32_t address);

    void io_write(bus_width_t width, uint32_t address, uint32_t data);
  };
}

#endif // __PSXACT_SPU_CORE_HPP__
