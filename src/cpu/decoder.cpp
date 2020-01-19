#include "cpu/core.hpp"

#include "util/int.hpp"
#include "util/uint.hpp"

using namespace psx::cpu;
using namespace psx::util;

uint32_t core_t::decode_iconst() {
  return int_t<16>::trunc(code);
}

uint32_t core_t::decode_uconst() {
  return uint_t<16>::trunc(code);
}

uint32_t core_t::decode_sa() {
  return uint_t<5>::trunc(code >> 6);
}

uint32_t core_t::decode_rd() {
  return uint_t<5>::trunc(code >> 11);
}

uint32_t core_t::decode_rt() {
  return uint_t<5>::trunc(code >> 16);
}

uint32_t core_t::decode_rs() {
  return uint_t<5>::trunc(code >> 21);
}
