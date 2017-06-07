#include <cassert>
#include <cstring>
#include <exception>
#include "bus.hpp"
#include "cdrom/cdrom_drive.hpp"
#include "cpu/cpu_core.hpp"
#include "dma/dma_core.hpp"
#include "gpu/gpu_core.hpp"
#include "input/input.hpp"
#include "spu/spu_core.hpp"
#include "timer/timer_core.hpp"
#include "utility.hpp"

static cdrom_state_t *cdrom_state;
static cpu_state_t *cpu_state;
static dma_state_t *dma_state;
static gpu_state_t *gpu_state;
static input_state_t *input_state;
static spu_state_t *spu_state;
static timer_state_t *timer_state;

static utility::memory_t<19> bios;
static utility::memory_t<21> wram;
static utility::memory_t<10> dmem;

void bus::initialize(system_state_t *state, const std::string &bios_file_name) {
  cdrom_state = &state->cdrom_state;
  cpu_state = &state->cpu_state;
  dma_state = &state->dma_state;
  gpu_state = &state->gpu_state;
  input_state = &state->input_state;
  spu_state = &state->spu_state;
  timer_state = &state->timer_state;

  memset(bios.b, 0, size_t(bios.size));
  memset(wram.b, 0, size_t(wram.size));
  memset(dmem.b, 0, size_t(dmem.size));

  utility::read_all_bytes(bios_file_name.c_str(), bios);
}

void bus::irq(int interrupt) {
  auto flag = 1 << interrupt;
  cpu::set_istat(cpu_state, cpu_state->i_stat | flag);
}

uint32_t bus::read(int width, uint32_t address) {
  if (utility::between<0x00000000, 0x007fffff>(address)) {
    switch (width) {
    case BUS_WIDTH_BYTE: return utility::read_byte(wram, address);
    case BUS_WIDTH_HALF: return utility::read_half(wram, address);
    case BUS_WIDTH_WORD: return utility::read_word(wram, address);
    }
  }

  if (utility::between<0x1fc00000, 0x1fc7ffff>(address)) {
    switch (width) {
    case BUS_WIDTH_BYTE: return utility::read_byte(bios, address);
    case BUS_WIDTH_HALF: return utility::read_half(bios, address);
    case BUS_WIDTH_WORD: return utility::read_word(bios, address);
    }
  }

  if (utility::between<0x1f800000, 0x1f8003ff>(address)) {
    switch (width) {
    case BUS_WIDTH_BYTE: return utility::read_byte(dmem, address);
    case BUS_WIDTH_HALF: return utility::read_half(dmem, address);
    case BUS_WIDTH_WORD: return utility::read_word(dmem, address);
    }
  }

  if (utility::between<0x1f801040, 0x1f80104f>(address)) {
    return input::io_read(input_state, width, address);
  }

  if (utility::between<0x1f801070, 0x1f801077>(address)) {
    return cpu::io_read(cpu_state, width, address);
  }

  if (utility::between<0x1f801080, 0x1f8010ff>(address)) {
    return dma::io_read(dma_state, width, address);
  }

  if (utility::between<0x1f801100, 0x1f80110f>(address) ||
      utility::between<0x1f801110, 0x1f80111f>(address) ||
      utility::between<0x1f801120, 0x1f80112f>(address)) {
    return timer::io_read(timer_state, width, address);
  }

  if (utility::between<0x1f801800, 0x1f801803>(address)) {
    return cdrom::io_read(cdrom_state, width, address);
  }

  if (utility::between<0x1f801810, 0x1f801817>(address)) {
    return gpu::io_read(gpu_state, width, address);
  }

  if (utility::between<0x1f801c00, 0x1f801fff>(address)) {
    return spu::io_read(spu_state, width, address);
  }

  if (utility::between<0x1f000000, 0x1f7fffff>(address) || // expansion region 1
      utility::between<0x1f802000, 0x1f802fff>(address) || // expansion region 2
      utility::between<0x1fa00000, 0x1fbfffff>(address)) { // expansion region 3
    return 0;
  }

  if (address == 0xfffe0130) {
    return 0;
  }

  printf("bus::read(%d, 0x%08x)\n", width, address);
  throw std::exception();
}

void bus::write(int width, uint32_t address, uint32_t data) {
  if (utility::between<0x00000000, 0x007fffff>(address)) {
    switch (width) {
    case BUS_WIDTH_BYTE: return utility::write_byte(wram, address, data);
    case BUS_WIDTH_HALF: return utility::write_half(wram, address, data);
    case BUS_WIDTH_WORD: return utility::write_word(wram, address, data);
    }
  }

  if (utility::between<0x1fc00000, 0x1fc7ffff>(address)) {
    printf("bios write: $%08x <- $%08x\n", address, data);
    return;
  }

  if (utility::between<0x1f800000, 0x1f8003ff>(address)) {
    switch (width) {
    case BUS_WIDTH_BYTE: return utility::write_byte(dmem, address, data);
    case BUS_WIDTH_HALF: return utility::write_half(dmem, address, data);
    case BUS_WIDTH_WORD: return utility::write_word(dmem, address, data);
    }
  }

  if (utility::between<0x1f801040, 0x1f80104f>(address)) {
    return input::io_write(input_state, width, address, data);
  }

  if (utility::between<0x1f801070, 0x1f801077>(address)) {
    return cpu::io_write(cpu_state, width, address, data);
  }

  if (utility::between<0x1f801080, 0x1f8010ff>(address)) {
    return dma::io_write(dma_state, width, address, data);
  }

  if (utility::between<0x1f801100, 0x1f80110f>(address) ||
      utility::between<0x1f801110, 0x1f80111f>(address) ||
      utility::between<0x1f801120, 0x1f80112f>(address)) {
    return timer::io_write(timer_state, width, address, data);
  }

  if (utility::between<0x1f801800, 0x1f801803>(address)) {
    return cdrom::io_write(cdrom_state, width, address, data);
  }

  if (utility::between<0x1f801810, 0x1f801817>(address)) {
    return gpu::io_write(gpu_state, width, address, data);
  }

  if (utility::between<0x1f801c00, 0x1f801fff>(address)) {
    return spu::io_write(spu_state, width, address, data);
  }

  if (utility::between<0x1f000000, 0x1f7fffff>(address) || // expansion region 1
      utility::between<0x1f802000, 0x1f802fff>(address) || // expansion region 2
      utility::between<0x1fa00000, 0x1fbfffff>(address)) { // expansion region 3
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
  case 0x1f801020: assert(data == 0x00031125 || data == 0x0000132c || data == 0x00001325); return;

  case 0x1f801060: assert(data == 0x00000b88); return;
  }

  if (address == 0xfffe0130) {
    // bus::write(2, 0xfffe0130, 0x00000804)
    // bus::write(2, 0xfffe0130, 0x00000800)
    // bus::write(2, 0xfffe0130, 0x0001e988)

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

  printf("bus::write(%d, 0x%08x, 0x%08x)\n", width, address, data);
  throw std::exception();
}
