#include "input.hpp"

static input::state_t state;

uint32_t input::bus_read(int width, uint32_t address) {
  switch (address) {
  case 0x1f801040:
    state.status &= ~(1 << 1);
    return 0xffffffff;

  case 0x1f801044:
    return state.status | (1 << 0) | (1 << 2);
  }

  return 0;
}

void input::bus_write(int width, uint32_t address, uint32_t data) {
  switch (address) {
  case 0x1f801040:
    state.status |= (1 << 1);
    break;
  }
}
