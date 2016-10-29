#include <cstdio>
#include "timer_core.hpp"

static timer::state_t state[3];

uint32_t timer::mmio_read(int size, uint32_t address) {
  auto channel = (address >> 4) & 3;
  switch ((address >> 2) & 3) {
    case 0: return state[channel].counter;
    case 1: return state[channel].control;
    case 2: return state[channel].target;
  }

  printf("timer::write_channel_%d(0x%08x)\n", channel, address);
  return 0;
}

void timer::mmio_write(int size, uint32_t address, uint32_t data) {
  auto channel = (address >> 4) & 3;
  switch ((address >> 2) & 3) {
    case 0: state[channel].counter = 0x00; return;
    case 1: state[channel].control = data; return;
    case 2: state[channel].target  = data; return;
  }

  printf("timer::write_channel_%d(0x%08x, 0x%08x)\n", channel, address, data);
}
