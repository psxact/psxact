#include "bus.hpp"
#include "cpu/cpu_core.hpp"
#include "dma/dma_core.hpp"
#include "gpu/gpu_core.hpp"
#include "spu/spu_core.hpp"
#include "utility.hpp"
#include "timer/timer_core.hpp"
#include "cdrom/cdrom_core.hpp"
#include <stdexcept>
#include <cstring>
#include <cassert>

utility::memory_t<19> bios;
utility::memory_t<21> wram;
utility::memory_t<10> dmem;

void bus::initialize(const std::string &bios_file_name, const std::string &game_file_name) {
  memset(bios.b, 0, size_t(bios.size));
  memset(wram.b, 0, size_t(wram.size));
  memset(dmem.b, 0, size_t(dmem.size));

  utility::read_all_bytes(bios_file_name.c_str(), bios);
}

void bus::irq_ack(int interrupt) {
  cpu::state.i_stat &= ~(1 << interrupt);
}

void bus::irq_req(int interrupt) {
  printf("bus::irq_req(%d)\n", interrupt);

  cpu::state.i_stat |= (1 << interrupt);
}

uint32_t bus::read_byte(uint32_t address) {
  if (utility::between<0x00000000, 0x007fffff>(address)) {
    return utility::read_byte(wram, address);
  }

  if (utility::between<0x1fc00000, 0x1fc7ffff>(address)) {
    return utility::read_byte(bios, address);
  }

  if (utility::between<0x1f800000, 0x1f8003ff>(address)) {
    return utility::read_byte(dmem, address);
  }

  if (address == 0x1f000084) {
    return 0;
  }

  if (utility::between<0x1f801800, 0x1f801803>(address)) {
    return cdrom::mmio_read(BYTE, address);
  }

  printf("unknown byte read: $%08x\n", address);
  throw std::runtime_error("unknown read");
}

uint32_t bus::read_half(uint32_t address) {
  if (utility::between<0x00000000, 0x007fffff>(address)) {
    return utility::read_half(wram, address);
  }

  if (utility::between<0x1fc00000, 0x1fc7ffff>(address)) {
    return utility::read_half(bios, address);
  }

  if (utility::between<0x1f800000, 0x1f8003ff>(address)) {
    return utility::read_half(dmem, address);
  }

  if (utility::between<0x1f801070, 0x1f801077>(address)) {
    return cpu::mmio_read(HALF, address);
  }

  if (utility::between<0x1f801c00, 0x1f801fff>(address)) {
    return spu::mmio_read(HALF, address);
  }

  if (address == 0x1f801120) {
    return 0;
  }

  printf("unknown half read: $%08x\n", address);
  throw std::runtime_error("unknown read");
}

uint32_t bus::read_word(uint32_t address) {
  if (utility::between<0x00000000, 0x007fffff>(address)) {
    return utility::read_word(wram, address);
  }

  if (utility::between<0x1fc00000, 0x1fc7ffff>(address)) {
    return utility::read_word(bios, address);
  }

  if (utility::between<0x1f800000, 0x1f8003ff>(address)) {
    return utility::read_word(dmem, address);
  }

  if (utility::between<0x1f801000, 0x1f801fff>(address)) {
    if (utility::between<0x1f801070, 0x1f80107f>(address)) {
      return cpu::mmio_read(WORD, address);
    }

    if (utility::between<0x1f801080, 0x1f8010ff>(address)) {
      return dma::mmio_read(WORD, address);
    }

    if (utility::between<0x1f801100, 0x1f80110f>(address) ||
        utility::between<0x1f801110, 0x1f80111f>(address) ||
        utility::between<0x1f801120, 0x1f80112f>(address)) {
      return timer::mmio_read(WORD, address);
    }

    if (utility::between<0x1f801800, 0x1f801803>(address)) {
      return cdrom::mmio_read(WORD, address);
    }

    if (utility::between<0x1f801810, 0x1f801817>(address)) {
      return gpu::mmio_read(WORD, address);
    }

    if (utility::between<0x1f801c00, 0x1f801fff>(address)) {
      return spu::mmio_read(WORD, address);
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

  printf("unknown word read: $%08x\n", address);
  throw std::runtime_error("unknown read");
}

void bus::write_byte(uint32_t address, uint32_t data) {
  if (utility::between<0x00000000, 0x007fffff>(address)) {
    return utility::write_byte(wram, address, data);
  }

  if (utility::between<0x1fc00000, 0x1fc7ffff>(address)) {
    printf("bios write: $%08x <- $%08x\n", address, data);
    return;
  }

  if (utility::between<0x1f800000, 0x1f8003ff>(address)) {
    return utility::write_byte(dmem, address, data);
  }

  if (utility::between<0x1f801800, 0x1f801803>(address)) {
    return cdrom::mmio_write(BYTE, address, data);
  }

  if (address == 0x1f802041) {
    return;
  }

  printf("unknown byte write: $%08x <- $%08x\n", address, data);
  throw std::runtime_error("unknown write");
}

void bus::write_half(uint32_t address, uint32_t data) {
  if (utility::between<0x00000000, 0x007fffff>(address)) {
    return utility::write_half(wram, address, data);
  }

  if (utility::between<0x1fc00000, 0x1fc7ffff>(address)) {
    printf("bios write: $%08x <- $%08x\n", address, data);
    return;
  }

  if (utility::between<0x1f800000, 0x1f8003ff>(address)) {
    return utility::write_half(dmem, address, data);
  }

  if (utility::between<0x1f801070, 0x1f801077>(address)) {
    return cpu::mmio_write(HALF, address, data);
  }

  if (utility::between<0x1f801c00, 0x1f801fff>(address)) {
    return spu::mmio_write(HALF, address, data);
  }

  if (address == 0x1f801048 || // joy_mode
      address == 0x1f80104a || // joy_ctrl
      address == 0x1f80104e || // joy_baud
      address == 0x1f801100 ||
      address == 0x1f801104 ||
      address == 0x1f801108 ||
      address == 0x1f801110 ||
      address == 0x1f801114 ||
      address == 0x1f801118 ||
      address == 0x1f801120 ||
      address == 0x1f801124 ||
      address == 0x1f801128) {
    return;
  }

  printf("unknown half write: $%08x <- $%08x\n", address, data);
  throw std::runtime_error("unknown write");
}

void bus::write_word(uint32_t address, uint32_t data) {
  if (utility::between<0x00000000, 0x007fffff>(address)) {
    return utility::write_word(wram, address, data);
  }

  if (utility::between<0x1fc00000, 0x1fc7ffff>(address)) {
    printf("bios write: $%08x <- $%08x\n", address, data);
    return;
  }

  if (utility::between<0x1f800000, 0x1f8003ff>(address)) {
    return utility::write_word(dmem, address, data);
  }

  if (utility::between<0x1f801000, 0x1f801fff>(address)) {
    if (utility::between<0x1f801070, 0x1f80107f>(address)) {
      return cpu::mmio_write(WORD, address, data);
    }

    if (utility::between<0x1f801080, 0x1f8010ff>(address)) {
      return dma::mmio_write(WORD, address, data);
    }

    if (utility::between<0x1f801100, 0x1f80110f>(address) ||
        utility::between<0x1f801110, 0x1f80111f>(address) ||
        utility::between<0x1f801120, 0x1f80112f>(address)) {
      return timer::mmio_write(WORD, address, data);
    }

    if (utility::between<0x1f801800, 0x1f801803>(address)) {
      return cdrom::mmio_write(WORD, address, data);
    }

    if (utility::between<0x1f801810, 0x1f801817>(address)) {
      return gpu::mmio_write(WORD, address, data);
    }

    if (utility::between<0x1f801c00, 0x1f801fff>(address)) {
      return spu::mmio_write(WORD, address, data);
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

  printf("unknown word write: $%08x <- $%08x\n", address, data);
  throw std::runtime_error("unknown write");
}
