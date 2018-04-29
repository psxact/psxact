#include "gpu/gpu.hpp"

#include <cstdio>


void gpu_t::draw_line() {
  printf("gpu::draw_line(0x%02x)\n", fifo.buffer[0] >> 24);
}
