// Copyright 2018 psxact

#include "input/device.hpp"

using namespace psx::input;

void device_t::start_ack_sequence() {
  ack = device_ack_t::LOW;
  ack_cycles = 100;
}

device_ack_t device_t::tick(int amount) {
  if (ack_cycles) {
    ack_cycles -= amount;

    if (ack_cycles <= 0) {
      ack_cycles = 0;
      ack = device_ack_t::HIGH;
    }
  }

  return ack;
}
