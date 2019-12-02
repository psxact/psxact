// Copyright 2018 psxact

#include "input/device.hpp"

using namespace psx::input;

device_t device_t::not_connected = device_t();

void device_t::frame() {
}

int device_t::send(int) {
  return 1;
}

void device_t::set_dtr(bool next_dtr) {
  dtr = next_dtr;
}

void device_t::start_dsr_pulse() {
  dsr_pending = true;
  dsr_cycles = 0;
}

void device_t::tick(int amount, device_dsr_t &dsr) {
  if (dsr_pending) {
    dsr_cycles += amount;

    if (dsr_cycles >= (DSR_DELAY_PERIOD + DSR_PULSE_PERIOD)) {
      dsr_pending = false;
    }
    else if (dsr_cycles >= DSR_DELAY_PERIOD) {
      dsr = device_dsr_t::LOW;
    }
  }
}