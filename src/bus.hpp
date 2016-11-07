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

  uint32_t read(int, uint32_t);

  void write(int, uint32_t, uint32_t);

  void bootstrap_exe();
}

#endif //PSXACT_BUS_HPP
