#include "input/device.hpp"

using namespace psx::input;

device device::not_connected = device();

void device::latch(const host_device &) {
}

int device::send(int) {
  return 1;
}

void device::set_dtr(bool next_dtr) {
  dtr = next_dtr;
}

void device::start_dsr_pulse() {
  dsr_pending = true;
  dsr_cycles = 0;
}

device_dsr device::tick(int amount, device_dsr dsr) {
  if (dsr_pending) {
    dsr_cycles += amount;

    if (dsr_cycles >= (DSR_DELAY_PERIOD + DSR_PULSE_PERIOD)) {
      dsr_pending = false;
    } else if (dsr_cycles >= DSR_DELAY_PERIOD) {
      return device_dsr::low;
    }
  }

  return dsr;
}
