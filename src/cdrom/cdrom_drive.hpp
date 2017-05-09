#ifndef __PSXACT_CDROM_DRIVE_HPP__
#define __PSXACT_CDROM_DRIVE_HPP__

#include <cstdint>
#include "../fifo.hpp"

namespace cdrom {
  struct state_t {
    uint32_t interrupt_enable;
    uint32_t interrupt_request;
    uint32_t index;
    uint32_t command;
    bool has_command;

    fifo_t<uint8_t, 16> args_fifo;
    fifo_t<uint8_t, 16> resp_fifo;
    fifo_t<uint8_t, 16> data_fifo; // actually bigger, not sure the exact size
  };

  extern state_t state;

  uint32_t io_read(int width, uint32_t address);

  void io_write(int width, uint32_t address, uint32_t data);

  void run();
}

#endif // __PSXACT_CDROM_DRIVE_HPP__
