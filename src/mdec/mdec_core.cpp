#include "mdec_core.hpp"

static uint32_t read_data(mdec_state_t *state) {
  printf("mdec::read_data()\n");
  return 0;
}

static uint32_t read_stat(mdec_state_t *state) {
  printf("mdec::read_stat()\n");
  return 0;
}

uint32_t mdec::io_read(mdec_state_t* state, int width, uint32_t address) {
  switch (address - 0x1f801820) {
  case 0:
    return read_data(state);

  case 4:
    return read_stat(state);
  }

  return 0;
}

static void write_command(mdec_state_t *state, uint32_t data) {
  printf("mdec::write_command(0x%08x)\n", data);
}

static void write_control(mdec_state_t *state, uint32_t data) {
  printf("mdec::write_control(0x%08x)\n", data);
}

void mdec::io_write(mdec_state_t *state, int width, uint32_t address, uint32_t data) {
  switch (address - 0x1f801820) {
  case 0:
    return write_command(state, data);

  case 4:
    return write_control(state, data);
  }
}
