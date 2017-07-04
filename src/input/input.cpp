#include "input.hpp"
#include "../utility.hpp"

uint32_t input::io_read(input_state_t *state, int width, uint32_t address) {
  if (utility::log_input) {
    printf("input::io_read(%d, 0x%08x)\n", width, address);
  }

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
  if (utility::log_input) {
    printf("input::io_write(%d, 0x%08x, 0x%08x)\n", width, address, data);
  }

  switch (address) {
  case 0x1f801040:
    state->status |= (1 << 1);
    break;
  }
}
