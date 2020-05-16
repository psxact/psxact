#include "memory-control.hpp"

#include <cassert>

using namespace psx;

memory_control_t::memory_control_t()
  : addressable_t("memctl", false) {
}

uint32_t memory_control_t::io_read(address_width_t width, uint32_t address) {
  switch (address) {
    case 0x1f801000: return 0x1f000000;
    case 0x1f801004: return 0x1f802000;
    case 0x1f801008: return 0x0013243f;
    case 0x1f80100c: return 0x00003022;
    case 0x1f801010: return 0x0013243f;
    case 0x1f801014: return 0x200931e1;
  //case 0x1f801018: return;
    case 0x1f80101c: return 0x00070777;
  //case 0x1f801020: return;

    case 0x1f801060: return 0x00000b88;
  }

  if (address == 0xfffe0130) {
    return biu;
  }

  return addressable_t::io_read(width, address);
}

void memory_control_t::io_write(address_width_t width, uint32_t address, uint32_t data) {
  switch (address) {
    case 0x1f801000:
      if (data == 0x1f000000) return;
      break;

    case 0x1f801004:
      if (data == 0x1f802000) return;
      break;

    case 0x1f801008:
      if (data == 0x0013243f) return;
      break;

    case 0x1f80100c:
      if (data == 0x00003022) return;
      break;

    case 0x1f801010:
      if (data == 0x0013243f) return;
      break;

    case 0x1f801014:
      if (data == 0x200931e1) return;
      if (data == 0x220931e1) return;
      break;

    case 0x1f801018:
      if (data == 0x00020843) return;
      if (data == 0x00020943) return;
      if (data == 0x21020843) return; // Oddworld - Abe's Oddysee
      break;

    case 0x1f80101c:
      if (data == 0x00070777) return;
      break;

    case 0x1f801020:
      if (data == 0x00031125) return;
      if (data == 0x0000132c) return;
      if (data == 0x00001323) return;
      if (data == 0x00001325) return;
      break;

    case 0x1f801060:
      if (data == 0x00000888) return; // Metal Gear Solid
      if (data == 0x00000b88) return;
      break;
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

    log("biu=%08x", data);
    biu = data;
    return;
  }

  return addressable_t::io_write(width, address, data);
}
