#ifndef UTIL_BCD_HPP_
#define UTIL_BCD_HPP_

#include <cstdint>
#include <cstdio>

namespace psx::util::bcd {

inline uint8_t of_dec(uint8_t value) {
  return uint8_t(((value / 10) * 16) + (value % 10));
}

inline uint8_t to_dec(uint8_t value) {
  return uint8_t(((value / 16) * 10) + (value % 16));
}

}  // namespace psx::util::bcd

#endif  // UTIL_BCD_HPP_
