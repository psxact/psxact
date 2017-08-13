#include "input_port.hpp"
#include "../utility.hpp"

namespace pgi = psxact::input;

void pgi::input_port::reset() {
  active = true;
  step = 0;
}

bool pgi::input_port::send_request(uint8_t request, uint8_t &response) {
  if (!active || !device) {
    response = 0xff;
    return false;
  }

  active = device->send_request(step, request, ref response);
  step++;

  return active;
}
