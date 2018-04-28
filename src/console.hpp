#ifndef __PSXACT_BUS_HPP__
#define __PSXACT_BUS_HPP__

#include <cstdint>
#include "interrupt-access.hpp"
#include "memory.hpp"
#include "memory-access.hpp"

class cdrom_t;

class counter_t;

class cpu_t;

class dma_t;

class gpu_t;

class input_t;

class mdec_t;

class spu_t;

class console_t
  : public memory_access_t
  , public interrupt_access_t {

  memory_t< kib(512) > bios;
  memory_t< mib(  2) > wram;
  memory_t< kib(  1) > dmem;

  cdrom_t *cdrom;
  counter_t *counter;
  cpu_t *cpu;
  dma_t *dma;
  gpu_t *gpu;
  input_t *input;
  mdec_t *mdec;
  spu_t *spu;

public:

  console_t(const char *bios_file_name, const char *game_file_name);

  void send(interrupt_type_t flag);

  uint32_t read(bus_width_t width, uint32_t address);

  void write(bus_width_t width, uint32_t address, uint32_t data);

  void run_for_one_frame(uint16_t **vram, int *w, int *h);
};


#endif // __psxact_console__
