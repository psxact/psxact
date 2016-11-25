#include <cstdio>
#include "timer_core.hpp"

uint32_t timer::bus_read(int width, uint32_t address) {
  printf("timer::bus_read(%d, 0x%08x)\n", width, address);
  return 0;
}

void timer::bus_write(int width, uint32_t address, uint32_t data) {
  printf("timer::bus_write(%d, 0x%08x, 0x%08x)\n", width, address, data);
}
