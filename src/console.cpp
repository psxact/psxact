// Copyright 2018 psxact

#include "console.hpp"

#include <cassert>
#include <cstring>
#include <exception>
#include "cdrom/core.hpp"
#include "timer/core.hpp"
#include "cpu/core.hpp"
#include "dma/core.hpp"
#include "exp/expansion1.hpp"
#include "exp/expansion2.hpp"
#include "exp/expansion3.hpp"
#include "gpu/core.hpp"
#include "input/core.hpp"
#include "mdec/core.hpp"
#include "spu/core.hpp"
#include "blob.hpp"
#include "limits.hpp"
#include "utility.hpp"

namespace psx {

console_t::console_t(args_t &args)
  : bios("bios")
  , wram("wram")
  , dmem("dmem")
  , bios_file_name(args.bios_file_name)
  , game_file_name(args.game_file_name) {
  cdrom = new cdrom::core_t(this, game_file_name, args.log_cdrom);
  timer = new timer::core_t(this, args.log_timer);
  cpu = new cpu::core_t(this, args.log_cpu);
  dma = new dma::core_t(this, this, args.log_dma);
  exp1 = new exp::expansion1_t();
  exp2 = new exp::expansion2_t();
  exp3 = new exp::expansion3_t();
  gpu = new gpu::core_t(args.log_gpu);
  input = new input::core_t(this, args.log_input);
  mdec = new mdec::core_t(args.log_mdec);
  spu = new spu::core_t(args.log_spu);

  bios.load_blob(bios_file_name);
  bios.io_write_word(0x6990, 0); // patch the bios to skip the boot-up animation

  is_exe = !!(strstr(game_file_name, ".exe"));
}

void console_t::send(interrupt_type_t flag) {
  int istat = cpu->get_istat() | static_cast<int>(flag);
  cpu->set_istat(istat);
}

memory_component_t *console_t::decode(uint32_t address) {
#define between(min, max) \
  limits::between<(min), (max)>(address)

  if (between(0x1f801800, 0x1f801803)) {
    if (is_exe) {
      is_exe = false;
      load_exe(game_file_name);
    }

    return cdrom;
  }

  if (between(0x00000000, 0x007fffff)) { return &wram; }
  if (between(0x1fc00000, 0x1fc7ffff)) { return &bios; }
  if (between(0x1f800000, 0x1f8003ff)) { return &dmem; }
  if (between(0x1f801040, 0x1f80104f)) { return input; }
  if (between(0x1f801070, 0x1f801077)) { return cpu; }
  if (between(0x1f801080, 0x1f8010ff)) { return dma; }
  if (between(0x1f801100, 0x1f80113f)) { return timer; }
  if (between(0x1f801800, 0x1f801803)) { return cdrom; }
  if (between(0x1f801810, 0x1f801817)) { return gpu; }
  if (between(0x1f801820, 0x1f801827)) { return mdec; }
  if (between(0x1f801c00, 0x1f801fff)) { return spu; }
  if (between(0x1f000000, 0x1f7fffff)) { return exp1; }
  if (between(0x1f802000, 0x1f802fff)) { return exp2; }
  if (between(0x1fa00000, 0x1fbfffff)) { return exp3; }

#undef between

  return nullptr;
}

uint32_t console_t::read_memory_control(int size, uint32_t address) {
  switch (address) {
    case 0x1f801000: return 0x1f000000;
    case 0x1f801004: return 0x1f802000;
    case 0x1f801008: return 0x0013243f;
    case 0x1f80100c: return 0x00003022;
    case 0x1f801010: return 0x0013243f;
    case 0x1f801014: return 0x200931e1;
//  case 0x1f801018: return;
    case 0x1f80101c: return 0x00070777;
//  case 0x1f801020: return;

    case 0x1f801060: return 0x00000b88;
  }

  if (address == 0xfffe0130) {
    return 0;
  }

  printf("system.read(%d, 0x%08x)\n", size, address);
  throw std::exception();
}

uint8_t console_t::read_byte(uint32_t address) {
  auto component = decode(address);

  return component != nullptr
    ? component->io_read_byte(address)
    : read_memory_control(1, address);
}

uint16_t console_t::read_half(uint32_t address) {
  auto component = decode(address);

  return component != nullptr
    ? component->io_read_half(address)
    : read_memory_control(2, address);
}

uint32_t console_t::read_word(uint32_t address) {
  auto component = decode(address);

  return component != nullptr
    ? component->io_read_word(address)
    : read_memory_control(4, address);
}

void console_t::write_memory_control(int size, uint32_t address, uint32_t data) {
  switch (address) {
    case 0x1f801000:
      return assert(data == 0x1f000000);

    case 0x1f801004:
      return assert(data == 0x1f802000);

    case 0x1f801008:
      return assert(data == 0x0013243f);

    case 0x1f80100c:
      return assert(data == 0x00003022);

    case 0x1f801010:
      return assert(data == 0x0013243f);

    case 0x1f801014:
      return assert(data == 0x200931e1);

    case 0x1f801018:
      return assert(data == 0x00020843 || data == 0x00020943);

    case 0x1f80101c:
      return assert(data == 0x00070777);

    case 0x1f801020:
      return assert(data == 0x00031125 || data == 0x0000132c || data == 0x00001323 || data == 0x00001325);

    case 0x1f801060:
      return assert(data == 0x00000b88);
  }

  if (address == 0xfffe0130) {
    // system.write(2, 0xfffe0130, 0x00000804)
    // system.write(2, 0xfffe0130, 0x00000800)
    // system.write(2, 0xfffe0130, 0x0001e988)

    //     17 :: nostr  - No Streaming
    //     16 :: ldsch  - Enable Load Scheduling
    //     15 :: bgnt   - Enable Bus Grant
    //     14 :: nopad  - No Wait State
    //     13 :: rdpri  - Enable Read Priority
    //     12 :: intp   - Interrupt Polarity
    //     11 :: is1    - Enable I-Cache Set 1
    //     10 :: is0    - Enable I-Cache Set 0
    //  9,  8 :: iblksz - I-Cache Refill Size
    //      7 :: ds     - Enable D-Cache
    //  5,  4 :: dblksz - D-Cache Refill Size
    //      3 :: ram    - Scratchpad RAM
    //      2 :: tag    - Tag Test Mode
    //      1 :: inv    - Invalidate Mode
    //      0 :: lock   - Lock Mode

    return;
  }

  printf("system.write(%d, 0x%08x, 0x%08x)\n", size, address, data);
  throw std::exception();
}

void console_t::write_byte(uint32_t address, uint8_t data) {
  auto component = decode(address);

  return (component != nullptr)
    ? component->io_write_byte(address, data)
    : write_memory_control(1, address, data);
}

void console_t::write_half(uint32_t address, uint16_t data) {
  auto component = decode(address);

  return (component != nullptr)
    ? component->io_write_half(address, data)
    : write_memory_control(2, address, data);
}

void console_t::write_word(uint32_t address, uint32_t data) {
  auto component = decode(address);

  return (component != nullptr)
    ? component->io_write_word(address, data)
    : write_memory_control(4, address, data);
}

void console_t::run_for_one_frame(uint16_t **vram, int *w, int *h) {
  constexpr int CYCLE_PER_CPU_TICK = 4;

  constexpr int CPU_FREQ = 33868800;
  constexpr int CPU_TICKS_PER_FRAME = CPU_FREQ / 60 / CYCLE_PER_CPU_TICK;

  for (int i = 0; i < CPU_TICKS_PER_FRAME; i++) {
    cpu->tick();

    timer->tick(CYCLE_PER_CPU_TICK);
    cdrom->tick(CYCLE_PER_CPU_TICK);
    input->tick(CYCLE_PER_CPU_TICK);
  }

  input->frame();

  send(interrupt_type_t::VBLANK);

  static const int w_lut[8] = { 256, 368, 320, 368, 512, 368, 640, 368 };
  static const int h_lut[2] = { 240, 480 };

  *w = w_lut[(gpu->status >> 16) & 7];
  *h = h_lut[(gpu->status >> 19) & 1];

  *vram = gpu->vram_data(
    gpu->display_area_x,
    gpu->display_area_y);
}

void console_t::load_exe(const char *game_file_name) {
  // load the exe into ram
  if (blob_t *blob = blob_t::from_file(game_file_name)) {
    cpu->set_pc(blob->read_word(0x10));
    cpu->set_register(4, 1);
    cpu->set_register(5, 0);
    cpu->set_register(28, blob->read_word(0x14));
    cpu->set_register(29, blob->read_word(0x30) + blob->read_word(0x34));
    cpu->set_register(30, blob->read_word(0x30) + blob->read_word(0x34));

    int text_start = blob->read_word(0x18);
    int text_count = blob->read_word(0x1c);

    printf("Loading executable into WRAM..\n");
    printf("  PC: $%08x\n", cpu->get_pc());
    printf("  GP: $%08x\n", cpu->get_register(28));
    printf("  SP: $%08x\n", cpu->get_register(29));
    printf("  FP: $%08x\n", cpu->get_register(30));

    for (int i = 0; i < text_count; i++) {
      wram.io_write_byte(text_start + i, blob->read_byte(0x800 + i));
    }
  }
}

}  // namespace psx
