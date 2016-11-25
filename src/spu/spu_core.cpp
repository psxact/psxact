#include <cstdio>
#include "spu_core.hpp"

uint32_t spu::bus_read(int width, uint32_t address) {
  printf("spu::bus_read(%d, 0x%08x)\n", width, address);
  return 0;
}

void spu::bus_write(int width, uint32_t address, uint32_t data) {
  printf("spu::bus_write(%d, 0x%08x, 0x%08x)\n", width, address, data);
}
