#include "spu/core.hpp"

using namespace psx::spu;

int core_t::dma_speed() {
  return 4;
}

bool core_t::dma_ready() {
  return true;
}

uint32_t core_t::dma_read() {
  return 0;
}

void core_t::dma_write(uint32_t val) {
  // TODO: there should be a FIFO that gets filled/emptied to transfer words.

  uint16_t lo = uint16_t(val);
  uint16_t hi = uint16_t(val >> 16);

  io_write_half(uint32_t(register_t::ram_data), lo);
  io_write_half(uint32_t(register_t::ram_data), hi);
}

uint16_t core_t::io_read_half(uint32_t address) {
  return get_register(register_t(address));
}

void core_t::io_write_half(uint32_t address, uint16_t data) {
  put_register(register_t(address), data);

  if (address >= 0x1f801c00 && address <= 0x1f801d7f) {
    auto &v = voices[(address >> 4) & 31];

    switch (address & 15) {
      case 0x0: v.volume_left = volume_t::create(data); return;
      case 0x2: v.volume_right = volume_t::create(data); return;
      case 0x4: v.pitch = data; return;
      case 0x6: v.start_address = sound_ram_address_t::create(data & ~1); return;
      case 0x8: return;
      case 0xA: return;
      case 0xC: return;
      case 0xE: v.loop_address = sound_ram_address_t::create(data & ~1); return;
    }
    return;
  }

  switch (register_t(address)) {
    case register_t::kon_lo:
      key_on &= 0xff0000;
      key_on |= data;
      break;

    case register_t::kon_hi:
      key_on &= 0x00ffff;
      key_on |= data << 16;
      break;

    case register_t::pmon_lo:
      pmon &= 0xff0000;
      pmon |= data;
      break;

    case register_t::pmon_hi:
      pmon &= 0x00ffff;
      pmon |= data << 16;
      break;

    case register_t::ram_addr_irq:
      ram_address_irq = sound_ram_address_t::create(data);
      log("IRQ address = 0x%08x", ram_address_irq);
      break;

    case register_t::ram_addr:
      ram_address = sound_ram_address_t::create(data);
      break;

    case register_t::ram_data:
      ram.write(ram_address, data);
      ram_address++;
      break;
  }
}

void core_t::io_write_word(uint32_t address, uint32_t data) {
  io_write_half(address & ~2, data);
  io_write_half(address | 2, data >> 16);
}
