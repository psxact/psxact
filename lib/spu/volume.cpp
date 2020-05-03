#include "spu/volume.hpp"

#include <cassert>

using namespace psx::spu;

int16_t volume_t::get_level() {
  return level;
}

void volume_t::put_level(uint16_t val) {
  mode = volume_mode_t((val >> 15) & 1);

  if (mode == volume_mode_t::fixed) {
    level = int16_t((val << 1) & 0xfffe);
  } else {
    assert(0 && "Volume sweep isn't supported yet.");
  }
}
