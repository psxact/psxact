#include "input_port.hpp"
#include "../utility.hpp"

using namespace psxact::input;

void port::reset() {
  active = true;
  step = 0;
}

bool port::send_request(uint8_t request, uint8_t *response) {
  if (!active || !device) {
    *response = 0xff;
    return false;
  }

  active = device->send_request(step, request, response);
  step++;

  return active;
}
