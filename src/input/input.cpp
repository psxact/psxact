#include <cstdio>
#include "input.hpp"

uint32_t input::bus_read(int width, uint32_t address) {
  printf("input::bus_read(%d, 0x%08x)\n", width, address);

  switch (address) {
    case 0x1f801044:
      return (1 << 0) | (1 << 2);
  }

  return 0;
}

void input::bus_write(int width, uint32_t address, uint32_t data) {
  printf("input::bus_write(%d, 0x%08x, 0x%08x)\n", width, address, data);
}
