#include "memory/memory-control.hpp"

#include <cassert>

using namespace psx::memory;

memory_control_t::memory_control_t()
  : addressable_t("memctl", false) {
}

uint32_t memory_control_t::io_read(address_width_t width, uint32_t address) {
  if (width == address_width_t::word) {
    if (address == 0xfffe0130) {
      return biu;
    }

    switch (address) {
      case 0x1f801000: return exp1_base;
      case 0x1f801004: return exp2_base;
      case 0x1f801008: return exp1_conf;
      case 0x1f80100c: return exp3_conf;
      case 0x1f801010: return bios_conf;
      case 0x1f801014: return spu_conf;
      case 0x1f801018: return cdc_conf;
      case 0x1f80101c: return exp2_conf;
      case 0x1f801020: return time_conf;
      case 0x1f801060: return wram_conf;
    }
  }

  return addressable_t::io_read(width, address);
}

void memory_control_t::io_write(address_width_t width, uint32_t address, uint32_t data) {
  if (width == address_width_t::word) {
    switch (address) {
      case 0x1f801000:
        exp1_base = (data & 0x00ffffff) | 0x1f000000;
        if (exp1_base == 0x1f000000) return;
        break;

      case 0x1f801004:
        exp2_base = (data & 0x00ffffff) | 0x1f000000;
        if (exp2_base == 0x1f802000) return;
        break;

      case 0x1f801008:
        exp1_conf = (data & 0xaf1fffff);
        if (exp1_conf == 0x0013243f) return;
        break;

      case 0x1f80100c:
        exp3_conf = (data & 0x2f1fffff);
        if (exp3_conf == 0x00003022) return;
        break;

      case 0x1f801010:
        bios_conf = (data & 0x2f1fffff);
        if (bios_conf == 0x0013243f) return;
        break;

      case 0x1f801014:
        spu_conf = (data & 0x2f1fffff);
        if (spu_conf == 0x200931e1) return;
        if (spu_conf == 0x220931e1) return;
        break;

      case 0x1f801018:
        cdc_conf = (data & 0x2f1fffff);
        if (cdc_conf == 0x00020843) return;
        if (cdc_conf == 0x00020943) return;
        if (cdc_conf == 0x21020843) return; // Oddworld - Abe's Oddysee
        break;

      case 0x1f80101c:
        exp2_conf = (data & 0x2f1fffff);
        if (exp2_conf == 0x00070777) return;
        break;

      case 0x1f801020:
        time_conf = (data & 0x0003ffff);
        if (time_conf == 0x00031125) return;
        if (time_conf == 0x0000132c) return;
        if (time_conf == 0x00001323) return;
        if (time_conf == 0x00001325) return;
        break;

      case 0x1f801060:
        wram_conf = (data & 0xffffffff);
        if (wram_conf == 0x00000888) return; // Metal Gear Solid
        if (wram_conf == 0x00000b88) return;
        break;
    }

    if (address == 0xfffe0130) {
      // [0xfffe0130] = 0x00000804
      // [0xfffe0130] = 0x00000800
      // [0xfffe0130] = 0x0001e988
      //
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

      log("biu=%08x", data);
      biu = data;
      return;
    }
  }

  return addressable_t::io_write(width, address, data);
}
