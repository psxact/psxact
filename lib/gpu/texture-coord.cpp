#include "gpu/texture-coord.hpp"

#include "util/uint.hpp"

using namespace psx::gpu;
using namespace psx::util;

texture_coord_t texture_coord_t::from_uint16(uint16_t val) {
  texture_coord_t coord;
  coord.u = uint_t<8>::trunc(val >> 0);
  coord.v = uint_t<8>::trunc(val >> 8);

  return coord;
}
