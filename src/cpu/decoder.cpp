#include "cpu/core.hpp"

#include "utility.hpp"


using namespace psx::cpu;

uint32_t core_t::decode_iconst() {
  return utility::sclip<16>(code);
}


uint32_t core_t::decode_uconst() {
  return utility::uclip<16>(code);
}


uint32_t core_t::decode_sa() {
  return utility::uclip<5>(code >> 6);
}


uint32_t core_t::decode_rd() {
  return utility::uclip<5>(code >> 11);
}


uint32_t core_t::decode_rt() {
  return utility::uclip<5>(code >> 16);
}


uint32_t core_t::decode_rs() {
  return utility::uclip<5>(code >> 21);
}
