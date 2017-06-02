#include "cpu_core.hpp"
#include "../state.hpp"

void cpu::cop2::op_cop(cpu_state_t *state) {
  printf("cpu::cop2::op_cop(0x%02x)\n", state->code & 0x3f);
}

void cpu::cop2::op_cfc(cpu_state_t *state) {
  printf("cpu::cop2::op_cfc(0x%02x, 0x%02x)\n",
         cpu::decoder::rt(state),
         cpu::decoder::rd(state));
}

void cpu::cop2::op_ctc(cpu_state_t *state) {
  printf("cpu::cop2::op_ctc(0x%02x, 0x%02x)\n",
         cpu::decoder::rt(state),
         cpu::decoder::rd(state));
}

void cpu::cop2::op_mfc(cpu_state_t *state) {
  printf("cpu::cop2::op_mfc(0x%02x, 0x%02x)\n",
         cpu::decoder::rt(state),
         cpu::decoder::rd(state));
}

void cpu::cop2::op_mtc(cpu_state_t *state) {
  printf("cpu::cop2::op_mtc(0x%02x, 0x%02x)\n",
         cpu::decoder::rt(state),
         cpu::decoder::rd(state));
}
