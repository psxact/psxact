#ifndef UTIL_RANGE_HPP_
#define UTIL_RANGE_HPP_

#include <cstdint>

namespace psx::range {

  template<uint32_t min, uint32_t max>
  bool between(uint32_t value) {
    return (value & ~(min ^ max)) == min;
  }
}

#endif
