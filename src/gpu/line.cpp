// Copyright 2018 psxact

#include "gpu/core.hpp"

#include <cstdio>

using psx::gpu::core_t;

void core_t::draw_line() {
  log("draw_line(0x%02x)", fifo.buffer[0] >> 24);
}
