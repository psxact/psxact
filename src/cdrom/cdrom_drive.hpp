#ifndef __PSXACT_CDROM_DRIVE_HPP__
#define __PSXACT_CDROM_DRIVE_HPP__

#include <cstdint>
#include "../fifo.hpp"
#include "../state.hpp"

namespace cdrom {
  uint32_t io_read(cdrom_state_t *state, int width, uint32_t address);

  void io_write(cdrom_state_t *state, int width, uint32_t address, uint32_t data);

  void run(cdrom_state_t *state);
}

#endif // __PSXACT_CDROM_DRIVE_HPP__
