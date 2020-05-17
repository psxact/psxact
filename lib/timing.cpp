#include "timing.hpp"

#include <cstdio>

static int32_t cpu_time;

void psx::timing::add_cpu_time(int32_t amount) {
  cpu_time += amount;
}

int32_t psx::timing::get_cpu_time() {
  return 3;
}

void psx::timing::reset_cpu_time() {
  cpu_time = 0;
}
