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

  uint32_t read_byte(uint32_t address);

  uint32_t read_half(uint32_t address);

  uint32_t read_word(uint32_t address);

  void write(int, uint32_t, uint32_t);
}

#endif //PSXACT_BUS_HPP
