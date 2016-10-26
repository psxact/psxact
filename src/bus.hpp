#ifndef __R3051_BUS_HPP__
#define __R3051_BUS_HPP__

#include <stdint.h>
#include <string>

enum bus_width_t {
  BYTE,
  HALF,
  WORD
};

namespace bus {
  void initialize(std::string file_name);

  uint32_t read(int, uint32_t);

  void write(int, uint32_t, uint32_t);
}

#endif
