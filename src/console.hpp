// Copyright 2018 psxact

#ifndef CONSOLE_HPP_
#define CONSOLE_HPP_

#include <cstdint>
#include "cdrom/core.hpp"
#include "cpu/core.hpp"
#include "dma/core.hpp"
#include "exp/expansion1.hpp"
#include "exp/expansion2.hpp"
#include "exp/expansion3.hpp"
#include "gpu/core.hpp"
#include "input/core.hpp"
#include "mdec/core.hpp"
#include "spu/core.hpp"
#include "timer/core.hpp"
#include "args.hpp"
#include "interrupt-access.hpp"
#include "memory.hpp"
#include "memory-access.hpp"

namespace psx {

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

  const char *bios_file_name;
  const char *game_file_name;
  bool is_exe;

 public:
  console_t(args_t &args);

  void send(interrupt_type_t flag);

  uint8_t read_byte(uint32_t address);

  uint16_t read_half(uint32_t address);

  uint32_t read_word(uint32_t address);

  void write_byte(uint32_t address, uint8_t data);

  void write_half(uint32_t address, uint16_t data);

  void write_word(uint32_t address, uint32_t data);

  void run_for_one_frame(uint16_t **vram, int *w, int *h);

 private:
  memory_component_t *decode(uint32_t address);

  uint32_t read_memory_control(int size, uint32_t address);

  void write_memory_control(int size, uint32_t address, uint32_t data);

  void load_exe(const char *game_file_name);
};

}  // namespace psx

#endif  // CONSOLE_HPP_
