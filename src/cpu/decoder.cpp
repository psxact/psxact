#include "cpu_core.hpp"
#include "../utility.hpp"

uint32_t cpu::decoder::iconst() {
  return utility::sclip<16>(cpu::state.code);
}

uint32_t cpu::decoder::uconst() {
  return utility::uclip<16>(cpu::state.code);
}

uint32_t cpu::decoder::sa() {
  return (cpu::state.code >>  6) & 31;
}

uint32_t cpu::decoder::rd() {
  return (cpu::state.code >> 11) & 31;
}

uint32_t cpu::decoder::rt() {
  return (cpu::state.code >> 16) & 31;
}

uint32_t cpu::decoder::rs() {
  return (cpu::state.code >> 21) & 31;
}
