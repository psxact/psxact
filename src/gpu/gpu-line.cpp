#include <cstdio>
#include "gpu/gpu.hpp"


void gpu_t::draw_line() {
  printf("gpu::draw_line(0x%02x)\n", fifo.buffer[0] >> 24);
}
