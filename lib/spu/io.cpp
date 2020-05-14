#include "spu/core.hpp"

using namespace psx::spu;

int core_t::dma_speed() {
  return 4;
}

bool core_t::dma_read_ready() {
  return true;
}

bool core_t::dma_write_ready() {
  return true;
}

uint32_t core_t::dma_read() {
  return 0;
}

void core_t::dma_write(uint32_t val) {
  // TODO: there should be a FIFO that gets filled/emptied to transfer words.

  uint16_t lo = uint16_t(val);
  uint16_t hi = uint16_t(val >> 16);

  io_write(address_width_t::half, uint32_t(register_t::ram_data), lo);
  io_write(address_width_t::half, uint32_t(register_t::ram_data), hi);
}

uint32_t core_t::io_read(address_width_t width, uint32_t address) {
  if (width == address_width_t::half) {
    if (address >= 0x1f801c00 && address <= 0x1f801d7f) {
      auto &v = voices[(address >> 4) & 31];

      switch (address & 15) {
        case 0x0: break;
        case 0x2: break;
        case 0x4: break;
        case 0x6: break;
        case 0x8: break;
        case 0xA: break;
        case 0xC: return v.adsr.get_level();
        case 0xE: return v.loop_address;
      }
    }

    return get_register(register_t(address));
  }

  return addressable_t::io_read(width, address);
}

void core_t::io_write(address_width_t width, uint32_t address, uint32_t data) {
  if (width == address_width_t::word) {
    io_write(address_width_t::half, address & ~2, data);
    io_write(address_width_t::half, address | 2, data >> 16);
    return;
  }

  if (width == address_width_t::half) {
    put_register(register_t(address), data);

    if (address >= 0x1f801c00 && address <= 0x1f801d7f) {
      auto &v = voices[(address >> 4) & 31];

      switch (address & 15) {
        case 0x0: v.volume_left.put_level(data); return;
        case 0x2: v.volume_right.put_level(data); return;
        case 0x4: v.pitch = data; return;
        case 0x6: v.start_address = sound_ram_address_t::create(data & ~1); return;
        case 0x8: v.adsr.put_config_lo(data); return;
        case 0xA: v.adsr.put_config_hi(data); return;
        case 0xC: v.adsr.put_level(int16_t(data)); return;
        case 0xE: v.loop_address = sound_ram_address_t::create(data & ~1); return;
      }
      return;
    }

    switch (register_t(address)) {
      case register_t::kon_lo:
        key_on &= 0xff0000;
        key_on |= data;
        return;

      case register_t::kon_hi:
        key_on &= 0x00ffff;
        key_on |= data << 16;
        return;

      case register_t::koff_lo:
        key_off &= 0xff0000;
        key_off |= data;
        return;

      case register_t::koff_hi:
        key_off &= 0x00ffff;
        key_off |= data << 16;
        return;

      case register_t::pmon_lo:
        pmon &= 0xff0000;
        pmon |= data;
        return;

      case register_t::pmon_hi:
        pmon &= 0x00ffff;
        pmon |= data << 16;
        return;

      case register_t::ram_addr_irq:
        ram_address_irq = sound_ram_address_t::create(data);
        log("IRQ address = 0x%08x", ram_address_irq);
        return;

      case register_t::ram_addr:
        ram_address = sound_ram_address_t::create(data);
        return;

      case register_t::ram_data:
        ram.write(ram_address, data);
        ram_address++;
        return;

      case register_t::cd_volume_left:
        cd_volume_left = int16_t(data);
        return;

      case register_t::cd_volume_right:
        cd_volume_right = int16_t(data);
        return;

      default:
        return;
    }
  }

  return addressable_t::io_write(width, address, data);
}
