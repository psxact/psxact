#ifndef __PSXACT_INPUT_HPP__
#define __PSXACT_INPUT_HPP__

#include "../system_core.hpp"
#include "../fifo.hpp"

namespace psxact {
namespace input {

  struct core {
    uint32_t status;

    int32_t baud_rate_factor;
    int32_t baud_rate_reload;
    int32_t baud_rate_timer;

    fifo_t<uint8_t, 3> rx_fifo;

    core();

    uint32_t io_read(bus_width_t width, uint32_t address);

    void io_write(bus_width_t width, uint32_t address, uint32_t data);

    void tick();

    void baud_reload();
  };

}
}

#endif // __PSXACT_INPUT_HPP__
