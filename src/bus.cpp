#include "bus.hpp"
#include "dma/dma_core.hpp"
#include "gpu/gpu_core.hpp"
#include "spu/spu_core.hpp"
#include "utility.hpp"
#include "timer/timer_core.hpp"
#include <stdexcept>
#include <cstring>
#include <cassert>

uint8_t *bios = new uint8_t[utility::kib<512>()];
uint8_t *wram = new uint8_t[utility::mib<  2>()];

void bus::initialize(std::string file_name) {
  memset(bios, 0, utility::kib<512>());
  memset(wram, 0, utility::mib<  2>());

  utility::read_all_bytes(file_name.c_str(), bios, 0, utility::kib<512>());
}

uint32_t bus::read(int size, uint32_t address) {
  if (utility::between<0x00000000, 0x007fffff>(address)) {
    switch (size) {
    case BYTE: return utility::read_byte(wram, address & 0x1fffff);
    case HALF: return utility::read_half(wram, address & 0x1ffffe);
    case WORD: return utility::read_word(wram, address & 0x1ffffc);
    }
  }

  if (utility::between<0x1fc00000, 0x1fc7ffff>(address)) {
    switch (size) {
      case BYTE: return utility::read_byte(bios, address & 0x7ffff);
      case HALF: return utility::read_half(bios, address & 0x7fffe);
      case WORD: return utility::read_word(bios, address & 0x7fffc);
    }
  }

  if (utility::between<0x1f800000, 0x1f8003ff>(address)) {
    printf("scratchpad read: $%08x\n", address);
    return 0;
  }

  if (utility::between<0x1f801000, 0x1f801fff>(address)) {
    if (utility::between<0x1f801080, 0x1f8010ff>(address)) {
      return dma::mmio_read(size, address);
    }

    if (utility::between<0x1f801100, 0x1f80110f>(address) ||
        utility::between<0x1f801110, 0x1f80111f>(address) ||
        utility::between<0x1f801120, 0x1f80112f>(address)) {
      return timer::mmio_read(size, address);
    }

    if (utility::between<0x1f801810, 0x1f801817>(address)) {
      return gpu::read(size, address);
    }

    if (utility::between<0x1f801c00, 0x1f801fff>(address)) {
      return spu::read(size, address);
    }

    printf("unhandled mmio read: $%08x\n", address);
    return 0;
  }

  if (address == 0xfffe0130) {
    return 0;
  }

  if (utility::between<0x1f000000, 0x1f7fffff>(address) || // expansion region 1
      utility::between<0x1f802000, 0x1f802fff>(address) || // expansion region 2
      utility::between<0x1fa00000, 0x1fbfffff>(address)) { // expansion region 3
    return 0;
  }

  printf("unknown read: $%08x\n", address);
  throw std::runtime_error("unknown read");
}

void bus::write(int size, uint32_t address, uint32_t data) {
  if (utility::between<0x00000000, 0x007fffff>(address)) {
    switch (size) {
    case BYTE: return utility::write_byte(wram, address & 0x1fffff, data);
    case HALF: return utility::write_half(wram, address & 0x1ffffe, data);
    case WORD: return utility::write_word(wram, address & 0x1ffffc, data);
    }
  }

  if (utility::between<0x1fc00000, 0x1fc7ffff>(address)) {
    printf("bios write: $%08x <- $%08x\n", address, data);
    return;
  }

  if (utility::between<0x1f800000, 0x1f8003ff>(address)) {
    printf("scratchpad write: $%08x <- $%08x\n", address, data);
    return;
  }

  if (utility::between<0x1f801000, 0x1f801fff>(address)) {
    if (utility::between<0x1f801080, 0x1f8010ff>(address)) {
      return dma::mmio_write(size, address, data);
    }

    if (utility::between<0x1f801100, 0x1f80110f>(address) ||
        utility::between<0x1f801110, 0x1f80111f>(address) ||
        utility::between<0x1f801120, 0x1f80112f>(address)) {
      return timer::mmio_write(size, address, data);
    }

    if (utility::between<0x1f801810, 0x1f801817>(address)) {
      return gpu::write(size, address, data);
    }

    if (utility::between<0x1f801c00, 0x1f801fff>(address)) {
      return spu::write(size, address, data);
    }

    switch (address) {
      case 0x1f801000: assert(data == 0x1f000000); return;
      case 0x1f801004: assert(data == 0x1f802000); return;
      case 0x1f801008: assert(data == 0x0013243f); return;
      case 0x1f80100c: assert(data == 0x00003022); return;
      case 0x1f801010: assert(data == 0x0013243f); return;
      case 0x1f801014: assert(data == 0x200931e1); return;
      case 0x1f801018: assert(data == 0x00020843); return;
      case 0x1f80101c: assert(data == 0x00070777); return;
      case 0x1f801020: assert(data == 0x00031125); return;

      case 0x1f801060: assert(data == 0x00000b88); return;
    }

    printf("unhandled mmio write: $%08x <- $%08x\n", address, data);
    return;
  }

  if (address == 0xfffe0130) {
    return;
  }

  if (utility::between<0x1f000000, 0x1f7fffff>(address) || // expansion region 1 mmio_write
      utility::between<0x1f802000, 0x1f802fff>(address) || // expansion region 2 mmio_write
      utility::between<0x1fa00000, 0x1fbfffff>(address)) { // expansion region 3 mmio_write
    return;
  }

  throw std::runtime_error("unknown write");
}
