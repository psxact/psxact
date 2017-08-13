#include "null_guest_input_device.hpp"

namespace pgi = psxact::input::guest;

bool pgi::null_device::send_request(uint8_t, uint8_t, uint8_t &response) {
  response = 0xff;
  return false;
}

void pgi::null_device::press(button_t) {}

void pgi::null_device::release(button_t) {}
