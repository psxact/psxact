// Copyright 2018 psxact

#ifndef CONSOLE_HPP_
#define CONSOLE_HPP_


#include <cstdint>
#include "interrupt-access.hpp"
#include "memory.hpp"
#include "memory-access.hpp"


namespace psx {

namespace cdrom {
class core_t;
}

namespace timer {
class core_t;
}

namespace cpu {
class core_t;
}

namespace dma {
class core_t;
}

namespace exp {
class expansion1_t;
class expansion2_t;
class expansion3_t;
}

namespace gpu {
class core_t;
}

namespace input {
class core_t;
}

namespace mdec {
class core_t;
}

namespace spu {
class core_t;
}

class console_t
  : public memory_access_t
  , public interrupt_access_t {
  memory_t< kib(512) > bios;
  memory_t< mib(2) > wram;
  memory_t< kib(1) > dmem;

  cdrom::core_t *cdrom;
  timer::core_t *timer;
  cpu::core_t *cpu;
  dma::core_t *dma;
  exp::expansion1_t *exp1;
  exp::expansion2_t *exp2;
  exp::expansion3_t *exp3;
  gpu::core_t *gpu;
  input::core_t *input;
  mdec::core_t *mdec;
  spu::core_t *spu;

 public:
  console_t(const char *bios_file_name, const char *game_file_name);

  void send(interrupt_type_t flag);

  uint32_t read_byte(uint32_t address);

  uint32_t read_half(uint32_t address);

  uint32_t read_word(uint32_t address);

  void write_byte(uint32_t address, uint32_t data);

  void write_half(uint32_t address, uint32_t data);

  void write_word(uint32_t address, uint32_t data);

  void run_for_one_frame(uint16_t **vram, int *w, int *h);

 private:
  memory_component_t *decode(uint32_t address);

  uint32_t read_memory_control(int size, uint32_t address);

  void write_memory_control(int size, uint32_t address, uint32_t data);
};

}  // namespace psx

#endif  // CONSOLE_HPP_
