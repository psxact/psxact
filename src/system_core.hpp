#ifndef __PSXACT_BUS_HPP__
#define __PSXACT_BUS_HPP__

#include <cstdint>
#include "memory.hpp"

namespace psxact {
  enum bus_width_t {
    BUS_WIDTH_BYTE,
    BUS_WIDTH_HALF,
    BUS_WIDTH_WORD
  };

  struct cdrom_core;

  struct cpu_core;

  struct dma_core;

  struct gpu_core;

  namespace input { struct input_core; }

  struct mdec_core;

  struct spu_core;

  struct timer_core;

  struct system_core {
    memory_t<19> bios;
    memory_t<21> wram;
    memory_t<10> dmem;

    cdrom_core *cdrom;
    cpu_core *cpu;
    dma_core *dma;
    gpu_core *gpu;
    input::input_core *input;
    mdec_core *mdec;
    spu_core *spu;
    timer_core *timer;

    system_core(const char *bios_file_name, const char *game_file_name);

    void irq(int32_t interrupt);

    uint32_t read(bus_width_t width, uint32_t address);

    void write(bus_width_t width, uint32_t address, uint32_t data);

    void run_for_one_frame();
  };

  extern system_core *system;
}

#endif // __PSXACT_BUS_HPP__
