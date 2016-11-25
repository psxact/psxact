#ifndef PSXACT_BUS_HPP
#define PSXACT_BUS_HPP

#include <stdint.h>
#include <string>

enum bus_width_t {
  BYTE,
  HALF,
  WORD
};

namespace bus {
  void initialize(const std::string &bios_file_name, const std::string &game_file_name);

  void irq(int interrupt);

  uint32_t read(int width, uint32_t address);

  void write(int width, uint32_t address, uint32_t data);
}

#endif //PSXACT_BUS_HPP
