#include "console.hpp"

#include <cassert>
#include <cstring>
#include <exception>
#include "cdrom/cdrom.hpp"
#include "counter/counter.hpp"
#include "cpu/cpu.hpp"
#include "dma/dma.hpp"
#include "expansion/exp1.hpp"
#include "expansion/exp2.hpp"
#include "expansion/exp3.hpp"
#include "gpu/gpu.hpp"
#include "input/input.hpp"
#include "mdec/mdec.hpp"
#include "spu/spu.hpp"
#include "limits.hpp"
#include "utility.hpp"


console_t::console_t(const char *bios_file_name, const char *game_file_name)
  : bios("bios")
  , dmem("dmem")
  , wram("wram") {

  cdrom = new cdrom_t(this, game_file_name);
  counter = new counter_t(this);
  cpu = new cpu_t(this);
  dma = new dma_t(this, this);
  exp1 = new exp1_t();
  exp2 = new exp2_t();
  exp3 = new exp3_t();
  gpu = new gpu_t();
  input = new input_t(this);
  mdec = new mdec_t();
  spu = new spu_t();

  bios.load_blob(bios_file_name);
}


void console_t::send(interrupt_type_t flag) {
  int istat = cpu->get_istat() | int(flag);
  cpu->set_istat(istat);
}


memory_component_t *console_t::decode(uint32_t address) {
#define between(min, max) \
  limits::between<(min), (max)>(address)

  if (between(0x00000000, 0x007fffff)) { return &wram; }
  if (between(0x1fc00000, 0x1fc7ffff)) { return &bios; }
  if (between(0x1f800000, 0x1f8003ff)) { return &dmem; }
  if (between(0x1f801040, 0x1f80104f)) { return input; }
  if (between(0x1f801070, 0x1f801077)) { return cpu; }
  if (between(0x1f801080, 0x1f8010ff)) { return dma; }
  if (between(0x1f801100, 0x1f80113f)) { return counter; }
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


uint32_t console_t::read_byte(uint32_t address) {
  auto component = decode(address);

  return component != nullptr
    ? component->io_read_byte(address)
    : read_memory_control(1, address)
    ;
}


uint32_t console_t::read_half(uint32_t address) {
  auto component = decode(address);

  return component != nullptr
    ? component->io_read_half(address)
    : read_memory_control(2, address)
    ;
}


uint32_t console_t::read_word(uint32_t address) {
  auto component = decode(address);

  return component != nullptr
    ? component->io_read_word(address)
    : read_memory_control(4, address)
    ;
}


void console_t::write_memory_control(int size, uint32_t address, uint32_t data) {
  switch (address) {
    case 0x1f801000: assert(data == 0x1f000000); return;
    case 0x1f801004: assert(data == 0x1f802000); return;
    case 0x1f801008: assert(data == 0x0013243f); return;
    case 0x1f80100c: assert(data == 0x00003022); return;
    case 0x1f801010: assert(data == 0x0013243f); return;
    case 0x1f801014: assert(data == 0x200931e1); return;
    case 0x1f801018: assert(data == 0x00020843 || data == 0x00020943); return;
    case 0x1f80101c: assert(data == 0x00070777); return;
    case 0x1f801020: assert(data == 0x00031125 || data == 0x0000132c || data == 0x00001323 || data == 0x00001325); return;

    case 0x1f801060: assert(data == 0x00000b88); return;
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


void console_t::write_byte(uint32_t address, uint32_t data) {
  auto component = decode(address);
  
  return (component != nullptr)
    ? component->io_write_byte(address, data)
    : write_memory_control(1, address, data)
    ;
}


void console_t::write_half(uint32_t address, uint32_t data) {
  auto component = decode(address);
  
  return (component != nullptr)
    ? component->io_write_half(address, data)
    : write_memory_control(2, address, data)
    ;
}


void console_t::write_word(uint32_t address, uint32_t data) {
  auto component = decode(address);
  
  return (component != nullptr)
    ? component->io_write_word(address, data)
    : write_memory_control(4, address, data)
    ;
}


void console_t::run_for_one_frame(uint16_t **vram, int *w, int *h) {
  const int ITERATIONS = 2;

  const int CPU_FREQ = 33868800;
  const int CYCLES_PER_FRAME = CPU_FREQ / 60 / ITERATIONS;

  for (int i = 0; i < CYCLES_PER_FRAME; i++) {
    cpu->tick();

    for (int j = 0; j < ITERATIONS; j++) {
      counter->tick();
      cdrom->tick();
      input->tick();
    }
  }

  send(interrupt_type_t::VBLANK);

  static const int w_lut[8] = { 256, 368, 320, 368, 512, 368, 640, 368 };
  static const int h_lut[2] = { 240, 480 };

  *w = w_lut[(gpu->status >> 16) & 7];
  *h = h_lut[(gpu->status >> 19) & 1];

  *vram = gpu->vram_data(
    gpu->display_area_x,
    gpu->display_area_y);
}
