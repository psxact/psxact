#include "cpu_core.hpp"
#include "../utility.hpp"

uint32_t cpu::decoder::iconst(cpu_state_t *state) {
  return utility::sclip<16>(state->code);
}

uint32_t cpu::decoder::uconst(cpu_state_t *state) {
  return utility::uclip<16>(state->code);
}

uint32_t cpu::decoder::sa(cpu_state_t *state) {
  return (state->code >>  6) & 31;
}

uint32_t cpu::decoder::rd(cpu_state_t *state) {
  return (state->code >> 11) & 31;
}

uint32_t cpu::decoder::rt(cpu_state_t *state) {
  return (state->code >> 16) & 31;
}

uint32_t cpu::decoder::rs(cpu_state_t *state) {
  return (state->code >> 21) & 31;
}
