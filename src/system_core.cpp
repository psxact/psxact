#include <cassert>
#include <cstring>
#include <exception>
#include "system_core.hpp"
#include "cdrom/cdrom_core.hpp"
#include "cpu/cpu_core.hpp"
#include "dma/dma_core.hpp"
#include "gpu/gpu_core.hpp"
#include "input/input_core.hpp"
#include "mdec/mdec_core.hpp"
#include "spu/spu_core.hpp"
#include "timer/timer_core.hpp"
#include "limits.hpp"
#include "utility.hpp"

namespace psxact {
  system_core::system_core(const char *bios_file_name, const char *game_file_name) {
    cdrom = new cdrom::core(game_file_name);
    cpu = new cpu::core();
    dma = new dma::core();
    gpu = new gpu::core();
    input = new input::core();
    mdec = new mdec::core();
    spu = new spu::core();
    timer = new timer::core();

    utility::read_all_bytes(bios_file_name, bios);
  }

  void system_core::irq(int32_t interrupt) {
    int32_t flag = 1 << interrupt;
    cpu->set_istat(cpu->i_stat | flag);
  }

  uint32_t system_core::read(bus_width_t width, uint32_t address) {
    if (limits::between<0x00000000, 0x007fffff>(address)) {
      switch (width) {
      case BUS_WIDTH_BYTE: return wram.read_byte(address);
      case BUS_WIDTH_HALF: return wram.read_half(address);
      case BUS_WIDTH_WORD: return wram.read_word(address);
      }
    }

    if (limits::between<0x1fc00000, 0x1fc7ffff>(address)) {
      switch (width) {
      case BUS_WIDTH_BYTE: return bios.read_byte(address);
      case BUS_WIDTH_HALF: return bios.read_half(address);
      case BUS_WIDTH_WORD: return bios.read_word(address);
      }
    }

    if (limits::between<0x1f800000, 0x1f8003ff>(address)) {
      switch (width) {
      case BUS_WIDTH_BYTE: return dmem.read_byte(address);
      case BUS_WIDTH_HALF: return dmem.read_half(address);
      case BUS_WIDTH_WORD: return dmem.read_word(address);
      }
    }

    if (limits::between<0x1f801040, 0x1f80104f>(address)) {
      return input->io_read(width, address);
    }

    if (limits::between<0x1f801070, 0x1f801077>(address)) {
      return cpu->io_read(width, address);
    }

    if (limits::between<0x1f801080, 0x1f8010ff>(address)) {
      return dma->io_read(width, address);
    }

    if (limits::between<0x1f801100, 0x1f80110f>(address) ||
        limits::between<0x1f801110, 0x1f80111f>(address) ||
        limits::between<0x1f801120, 0x1f80112f>(address)) {
      return timer->io_read(width, address);
    }

    if (limits::between<0x1f801800, 0x1f801803>(address)) {
      return cdrom->io_read(width, address);
    }

    if (limits::between<0x1f801810, 0x1f801817>(address)) {
      return gpu->io_read(width, address);
    }

    if (limits::between<0x1f801820, 0x1f801827>(address)) {
      return mdec->io_read(width, address);
    }

    if (limits::between<0x1f801c00, 0x1f801fff>(address)) {
      return spu->io_read(width, address);
    }

    if (limits::between<0x1f000000, 0x1f7fffff>(address) || // expansion region 1
        limits::between<0x1f802000, 0x1f802fff>(address) || // expansion region 2
        limits::between<0x1fa00000, 0x1fbfffff>(address)) { // expansion region 3
      return 0;
    }

    switch (address) {
  //case 0x1f801000: return 0x1f000000;
  //case 0x1f801004: return 0x1f802000;
  //case 0x1f801008: return 0x0013243f;
  //case 0x1f80100c: return 0x00003022;
  //case 0x1f801010: return 0x0013243f;
    case 0x1f801014: return 0x200931e1;
  //case 0x1f801018: return;
  //case 0x1f80101c: return 0x00070777;
  //case 0x1f801020: return;

  //case 0x1f801060: return 0x00000b88;
    }

    if (address == 0xfffe0130) {
      return 0;
    }

    printf("system.read(%d, 0x%08x)\n", width, address);
    throw std::exception();
  }

  void system_core::write(bus_width_t width, uint32_t address, uint32_t data) {
    if (limits::between<0x00000000, 0x007fffff>(address)) {
      switch (width) {
      case BUS_WIDTH_BYTE: return wram.write_byte(address, data);
      case BUS_WIDTH_HALF: return wram.write_half(address, data);
      case BUS_WIDTH_WORD: return wram.write_word(address, data);
      }
    }

    if (limits::between<0x1fc00000, 0x1fc7ffff>(address)) {
      printf("bios write: $%08x <- $%08x\n", address, data);
      return;
    }

    if (limits::between<0x1f800000, 0x1f8003ff>(address)) {
      switch (width) {
      case BUS_WIDTH_BYTE: return dmem.write_byte(address, data);
      case BUS_WIDTH_HALF: return dmem.write_half(address, data);
      case BUS_WIDTH_WORD: return dmem.write_word(address, data);
      }
    }

    if (limits::between<0x1f801040, 0x1f80104f>(address)) {
      return input->io_write(width, address, data);
    }

    if (limits::between<0x1f801070, 0x1f801077>(address)) {
      return cpu->io_write(width, address, data);
    }

    if (limits::between<0x1f801080, 0x1f8010ff>(address)) {
      return dma->io_write(width, address, data);
    }

    if (limits::between<0x1f801100, 0x1f80110f>(address) ||
        limits::between<0x1f801110, 0x1f80111f>(address) ||
        limits::between<0x1f801120, 0x1f80112f>(address)) {
      return timer->io_write(width, address, data);
    }

    if (limits::between<0x1f801800, 0x1f801803>(address)) {
      // return load_executable();
      return cdrom->io_write(width, address, data);
    }

    if (limits::between<0x1f801810, 0x1f801817>(address)) {
      return gpu->io_write(width, address, data);
    }

    if (limits::between<0x1f801820, 0x1f801827>(address)) {
      return mdec->io_write(width, address, data);
    }

    if (limits::between<0x1f801c00, 0x1f801fff>(address)) {
      return spu->io_write(width, address, data);
    }

    if (limits::between<0x1f000000, 0x1f7fffff>(address) || // expansion region 1
        limits::between<0x1f802000, 0x1f802fff>(address) || // expansion region 2
        limits::between<0x1fa00000, 0x1fbfffff>(address)) { // expansion region 3
      return;
    }

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

    printf("system.write(%d, 0x%08x, 0x%08x)\n", width, address, data);
    throw std::exception();
  }

  void system_core::run_for_one_frame(int *x, int *y, int *w, int *h) {
    const int ITERATIONS = 2;

    const int CPU_FREQ = 33868800;
    const int CYCLES_PER_FRAME = CPU_FREQ / 60 / ITERATIONS;

    for (int i = 0; i < CYCLES_PER_FRAME; i++) {
      cpu->tick();

      for (int j = 0; j < ITERATIONS; j++) {
        timer->tick();
        cdrom->tick();
        input->tick();
      }
    }

    irq(0);

    *x = (gpu->display_area_x);
    *y = (gpu->display_area_y);
    //*w = (gpu->display_area_x2 - gpu->display_area_x1) / 4;
    //*h = (gpu->display_area_y2 - gpu->display_area_y1);

    switch ((gpu->status >> 17) & 3) {
    case 0: *w = 256; break;
    case 1: *w = 320; break;
    case 2: *w = 512; break;
    case 3: *w = 640; break;
    }

    switch ((gpu->status >> 19) & 1) {
    case 0: *h = 240; break;
    case 1: *h = 480; break;
    }
  }
}
