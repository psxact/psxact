#include "spu/volume.hpp"

#include "util/panic.hpp"

using namespace psx::spu;

int16_t volume::get_level() {
  return level;
}

void volume::put_level(uint16_t val) {
  mode = volume_mode((val >> 15) & 1);

  if (mode == volume_mode::fixed) {
    level = int16_t((val << 1) & 0xfffe);
  } else {
    PANIC("Volume sweep isn't supported yet.");
  }
}
