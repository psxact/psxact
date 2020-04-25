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
#include "addressable.hpp"
#include "args.hpp"
#include "interruptible.hpp"
#include "memory.hpp"

namespace psx {

class console_t final
  : public addressable_t
  , public interruptible_t {
  memory_t< kib(512) > *bios;
  memory_t< mib(2) > *wram;

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

  ~console_t();

  void interrupt(interrupt_type_t flag);

  uint8_t io_read_byte(uint32_t address);

  uint16_t io_read_half(uint32_t address);

  uint32_t io_read_word(uint32_t address);

  void io_write_byte(uint32_t address, uint8_t data);

  void io_write_half(uint32_t address, uint16_t data);

  void io_write_word(uint32_t address, uint32_t data);

  void run_for_one_frame();

  void get_audio_params(int16_t **sound, int *len);

  void get_video_params(uint16_t **vram, int *w, int *h);

 private:
  addressable_t *decode(uint32_t address);

  uint32_t read_memory_control(int size, uint32_t address);

  void write_memory_control(int size, uint32_t address, uint32_t data);

  void load_exe(const char *game_file_name);
};

}  // namespace psx

#endif  // CONSOLE_HPP_
