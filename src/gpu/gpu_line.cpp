#include <cstdio>
#include "gpu_core.hpp"

namespace pg = psxact::gpu;

void pg::core::draw_line() {
  printf("gpu::draw_line(0x%02x)\n", fifo.buffer[0] >> 24);
}
