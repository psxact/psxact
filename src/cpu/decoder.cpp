#include "cpu_core.hpp"
#include "../utility.hpp"

uint32_t cpu::decoder::iconst(cpu_state_t &state) {
  return utility::sclip<16>(state.code);
}

uint32_t cpu::decoder::uconst(cpu_state_t &state) {
  return utility::uclip<16>(state.code);
}

uint32_t cpu::decoder::sa(cpu_state_t &state) {
  return utility::uclip<5>(state.code >>  6);
}

uint32_t cpu::decoder::rd(cpu_state_t &state) {
  return utility::uclip<5>(state.code >> 11);
}

uint32_t cpu::decoder::rt(cpu_state_t &state) {
  return utility::uclip<5>(state.code >> 16);
}

uint32_t cpu::decoder::rs(cpu_state_t &state) {
  return utility::uclip<5>(state.code >> 21);
}
