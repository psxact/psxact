#include "digital_device.hpp"

using namespace psxact::input::guest;

digital_device::digital_device()
    : data(0xffff) {
}

bool digital_device::send_request(int step, uint8_t request, uint8_t *response) {
  switch (step) {
  case 0:
    *response = 0xff;
    return request == 0x01;

  case 1:
    *response = 0x41;
    return request == 0x42;

  case 2:
    *response = 0x5a;
    return true;

  case 3:
    *response = uint8_t(data >> 0);
    return true;

  case 4:
    *response = uint8_t(data >> 8);
    return false;

  default:
    *response = 0xff;
    return false;
  }
}

static int get_button_mask(int button) {
  return 1 << button;
}

void digital_device::press(button_t button) {
  data = data & ~get_button_mask(button);
}

void digital_device::release(button_t button) {
  data = data | get_button_mask(button);
}
