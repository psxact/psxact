#ifndef PSXACT_INPUT_HPP
#define PSXACT_INPUT_HPP

#include <cstdint>

namespace input {
  uint32_t bus_read(int width, uint32_t address);

  void bus_write(int width, uint32_t address, uint32_t data);
}

#endif //PSXACT_INPUT_HPP
