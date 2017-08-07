#include "input_core.hpp"
#include "../utility.hpp"

static uint32_t get_index(uint32_t address) {
  return address - 0x1f801040;
}

uint32_t input_core::io_read(bus_width_t width, uint32_t address) {
  if (utility::log_input) {
    printf("input::io_read(%d, 0x%08x)\n", width, address);
  }

  switch (get_index(address)) {
  case 0:
    status &= ~2;
    return 0xffffffff;

  case 4:
    return status | 1 | 4 | (baud_rate_timer << 11);
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

void input_core::io_write(bus_width_t width, uint32_t address, uint32_t data) {
  if (utility::log_input) {
    printf("input::io_write(%d, 0x%08x, 0x%08x)\n", width, address, data);
  }

  switch (get_index(address)) {
  case 0:
    status |= 2;
    break;

  case 8:
    baud_rate_factor = get_baud_rate_factor(data);
    break;

  case 10:
    break;

  case 14:
    baud_rate_reload = data & 0xffff;
    break;
  }
}

void input_core::baud_reload() {
  auto reload = baud_rate_reload;
  auto factor = baud_rate_factor;

  baud_rate_timer = reload * factor;
}

void input_core::init() {
  baud_rate_factor = 1;
  baud_rate_reload = 0x0088;

  baud_reload();
}

void input_core::tick() {
  baud_rate_timer--;

  if (baud_rate_timer == 0) {
    baud_reload();
  }
}
