#include "spu/volume.hpp"

using namespace psx::spu;

int32_t volume_t::apply(int32_t sample) {
  return (level * sample) >> 15;
}

volume_t volume_t::create(uint16_t val) {
  volume_t result;
  result.fixed = (val & (1 << 15)) == 0;

  if (result.fixed) {
    result.level = int16_t((val << 1) & 0xfffe);
  }

  // TODO: implement sweep

  return result;
}
