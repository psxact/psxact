#include "gpu_core.hpp"

void gpu::draw_line(gpu_state_t &state) {
  printf("gpu::draw_line(0x%02x)\n", state.fifo.buffer[0] >> 24);
}
