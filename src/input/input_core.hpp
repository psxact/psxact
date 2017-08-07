#ifndef __PSXACT_INPUT_HPP__
#define __PSXACT_INPUT_HPP__

#include "../bus.hpp"

struct input_core {
  uint32_t status;

  int32_t baud_rate_factor;
  int32_t baud_rate_reload;
  int32_t baud_rate_timer;

  uint32_t io_read(bus_width_t width, uint32_t address);

  void io_write(bus_width_t width, uint32_t address, uint32_t data);

  void init();

  void tick();

  void baud_reload();
};

#endif // __PSXACT_INPUT_HPP__
