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

  namespace cdrom {
    struct core;
  }

  namespace cpu {
    struct core;
  }

  namespace dma {
    struct core;
  }

  namespace gpu {
    struct core;
  }

  namespace input {
    struct core;
  }

  namespace mdec {
    struct core;
  }

  namespace spu {
    struct core;
  }

  namespace timer {
    struct core;
  }

  struct system_core {
    memory_t<19> bios;
    memory_t<21> wram;
    memory_t<10> dmem;

    cdrom::core *cdrom;
    cpu::core *cpu;
    dma::core *dma;
    gpu::core *gpu;
    input::core *input;
    mdec::core *mdec;
    spu::core *spu;
    timer::core *timer;

    system_core(const char *bios_file_name, const char *game_file_name);

    void irq(int32_t interrupt);

    uint32_t read(bus_width_t width, uint32_t address);

    void write(bus_width_t width, uint32_t address, uint32_t data);

    void run_for_one_frame(int *x, int *y, int *w, int *h);
  };

  extern system_core *system;
}

#endif // __PSXACT_BUS_HPP__
