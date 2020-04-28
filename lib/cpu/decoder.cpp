#include "cpu/core.hpp"

#include "util/int.hpp"
#include "util/uint.hpp"

using namespace psx::cpu;
using namespace psx::util;

uint32_t core_t::decode_iconst() const {
  return int_t<16>::trunc(get_code());
}

uint32_t core_t::decode_uconst() const {
  return uint_t<16>::trunc(get_code());
}

uint32_t core_t::decode_sa() const {
  return uint_t<5>::trunc(get_code() >> 6);
}

uint32_t core_t::decode_rd() const {
  return uint_t<5>::trunc(get_code() >> 11);
}

uint32_t core_t::decode_rt() const {
  return uint_t<5>::trunc(get_code() >> 16);
}

uint32_t core_t::decode_rs() const {
  return uint_t<5>::trunc(get_code() >> 21);
}
