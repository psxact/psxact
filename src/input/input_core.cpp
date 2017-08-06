#include "input_core.hpp"
#include "../utility.hpp"

static uint32_t get_index(uint32_t address) {
  return address - 0x1f801040;
}

uint32_t input::io_read(input_state_t &state, bus::bus_width_t width, uint32_t address) {
  if (utility::log_input) {
    printf("input::io_read(%d, 0x%08x)\n", width, address);
  }

  switch (get_index(address)) {
  case 0:
    state.status &= ~2;
    return 0xffffffff;

  case 4:
    return state.status | 1 | 4 | (state.baud_rate_timer << 11);
  }

  return 0;
}

static int32_t get_baud_rate_factor(uint32_t data) {
  switch (data & 3) {
  case 0: return  1;
  case 1: return  1;
  case 2: return 16;
  case 3: return 64;
  }

  return 0;
}

void input::io_write(input_state_t &state, bus::bus_width_t width, uint32_t address, uint32_t data) {
  if (utility::log_input) {
    printf("input::io_write(%d, 0x%08x, 0x%08x)\n", width, address, data);
  }

  switch (get_index(address)) {
  case 0:
    state.status |= 2;
    break;

  case 8:
    state.baud_rate_factor = get_baud_rate_factor(data);
    break;

  case 10:
    break;

  case 14:
    state.baud_rate_reload = data & 0xffff;
    break;
  }
}

static void baud_reload(input_state_t &state) {
  auto reload = state.baud_rate_reload;
  auto factor = state.baud_rate_factor;

  state.baud_rate_timer = reload * factor;
}

void input::init(input_state_t &state) {
  state.baud_rate_factor = 1;
  state.baud_rate_reload = 0x0088;

  baud_reload(state);
}

void input::tick(input_state_t &state) {
  state.baud_rate_timer--;

  if (state.baud_rate_timer == 0) {
    baud_reload(state);
  }
}
