#include <cassert>
#include "cdrom_core.hpp"
#include "../bus.hpp"

cdrom::state_t cdrom::state;

uint32_t cdrom::mmio_read(int size, uint32_t address) {
  assert(size == BYTE);

  switch (address - 0x1f801800) {
    case 0: return 0; // status register
    case 1: return 0; // response fifo
    case 2: return 0; // data fifo

    case 3:
      switch (state.index) {
        case 0:
        case 2: return 0; // interrupt enable register

        case 1:
        case 3: return 0; // interrupt flag register
      }
      return 0;
  }

  return 0;
}

void cdrom::mmio_write(int size, uint32_t address, uint32_t data) {
  assert(size == BYTE);

  switch (address - 0x1f801800) {
    case 0:
      state.index = data & 3;
      return;

    case 1:
      switch (state.index) {
        case 0: // command register

          break;

        case 1: break; // sound map data out
        case 2: break; // sound map coding info
        case 3: break; // audio volume for cd-right to spu-right
      }
      break;

    case 2:
      switch (state.index) {
        case 0: state.parameter_fifo.push_back(uint8_t(data)); break; // parameter fifo
        case 1: break; // interrupt enable register
        case 2: break; // audio volume for cd-left to spu-left
        case 3: break; // audio volume for cd-right to spu-left
      }
      break;

    case 3:
      switch (state.index) {
        case 0: break; // request register
        case 1: break; // interrupt flag register
        case 2: break; // audio volume for cd-left to spu-right
        case 3: break; // apply volume changes
      }
      break;
  }
}
