#ifndef PSXACT_CDROM_CORE_HPP
#define PSXACT_CDROM_CORE_HPP

#include <cstdint>
#include <deque>

namespace cdrom {
  struct state_t {
    uint32_t interrupt_enable;
    uint32_t interrupt_request;
    uint32_t index;
    uint32_t command;
    bool has_command;

    std::deque<uint8_t> args_fifo;
    std::deque<uint8_t> resp_fifo;
    std::deque<uint8_t> data_fifo;
  };

  extern state_t state;

  uint32_t bus_read(int width, uint32_t address);

  void bus_write(int width, uint32_t address, uint32_t data);

  void run();
}

#endif //PSXACT_CDROM_CORE_HPP
