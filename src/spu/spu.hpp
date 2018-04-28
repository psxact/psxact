#ifndef __psxact_spu__
#define __psxact_spu__


#include "console.hpp"
#include "memory.hpp"
#include "memory-component.hpp"


class spu_t : public memory_component_t {

  uint16_t control;
  uint16_t status;

  uint16_t registers[24][8];

  memory_t< kib(512) > sound_ram;

  uint32_t sound_ram_address;
  uint16_t sound_ram_address_latch;
  uint16_t sound_ram_transfer_control;

public:

  spu_t();

  uint32_t io_read_half(uint32_t address);

  void io_write_half(uint32_t address, uint32_t data);

};


#endif // __psxact_spu__
