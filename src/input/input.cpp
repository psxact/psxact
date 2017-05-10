#include "input.hpp"

uint32_t input::io_read(input_state_t *state, int width, uint32_t address) {
  switch (address) {
  case 0x1f801040:
    state->status &= ~(1 << 1);
    return 0xffffffff;

  case 0x1f801044:
    return state->status | (1 << 0) | (1 << 2);
  }

  return 0;
}

void input::io_write(input_state_t *state, int width, uint32_t address, uint32_t data) {
  switch (address) {
  case 0x1f801040:
    state->status |= (1 << 1);
    break;
  }
}
