#ifndef PSXACT_CDROM_CORE_HPP
#define PSXACT_CDROM_CORE_HPP

#include <cstdint>
#include <deque>

namespace cdrom {
  struct state_t {
    uint32_t interrupt_enable;
    uint32_t interrupt_request;
    uint32_t index;

    std::deque<uint8_t> args_fifo;
    std::deque<uint8_t> resp_fifo;
    std::deque<uint8_t> data_fifo;
  };

  extern state_t state;

  uint32_t mmio_read(int size, uint32_t address);

  void mmio_write(int size, uint32_t address, uint32_t data);
}

#endif //PSXACT_CDROM_CORE_HPP
