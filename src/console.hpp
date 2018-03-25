#ifndef __PSXACT_BUS_HPP__
#define __PSXACT_BUS_HPP__

#include <cstdint>
#include "interrupt_access.hpp"
#include "memory.hpp"
#include "memory_access.hpp"

struct cdrom_t;

struct counter_t;

struct cpu_t;

struct dma_t;

struct gpu_t;

struct input_t;

struct mdec_t;

struct spu_t;

struct console_t
  : public memory_access_t
  , public interrupt_access_t {

  memory_t<19> bios;
  memory_t<21> wram;
  memory_t<10> dmem;

  cdrom_t *cdrom;
  counter_t *counter;
  cpu_t *cpu;
  dma_t *dma;
  gpu_t *gpu;
  input_t *input;
  mdec_t *mdec;
  spu_t *spu;

  console_t(const char *bios_file_name, const char *game_file_name);

  void send(interrupt_type_t flag);

  uint32_t read(bus_width_t width, uint32_t address);

  void write(bus_width_t width, uint32_t address, uint32_t data);

  void run_for_one_frame(int *x, int *y, int *w, int *h);
};


#endif // __psxact_console__
