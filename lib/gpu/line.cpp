#include "gpu/core.hpp"

#include <cstdio>

using namespace psx::gpu;

void core_t::draw_line() {
  log("draw_line(0x%02x)", fifo.buffer[0] >> 24);
}
