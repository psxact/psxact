// Copyright 2018 psxact

#include "gpu/core.hpp"

#include <cstdio>

using psx::gpu::core_t;

void core_t::draw_line() {
  printf("gpu::draw_line(0x%02x)\n", fifo.buffer[0] >> 24);
}
