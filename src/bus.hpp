#ifndef __PSXACT_BUS_HPP__
#define __PSXACT_BUS_HPP__

#include <stdint.h>
#include <string>

namespace bus {
  enum bus_width_t {
    BUS_WIDTH_BYTE,
    BUS_WIDTH_HALF,
    BUS_WIDTH_WORD
  };

  void initialize(const std::string &bios_file_name, const std::string &game_file_name);

  void irq(int interrupt);

  uint32_t read(int width, uint32_t address);

  void write(int width, uint32_t address, uint32_t data);
}

#endif // __PSXACT_BUS_HPP__
