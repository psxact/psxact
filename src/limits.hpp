#ifndef LIMITS_HPP_
#define LIMITS_HPP_

#include <cstdint>

namespace psx {

namespace limits {
  template<uint32_t min, uint32_t max>
  bool between(uint32_t value) {
    return (value & ~(min ^ max)) == min;
  }
}

}  // namespace psx

#endif  // LIMITS_HPP_
