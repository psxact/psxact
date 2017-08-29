#ifndef __PSXACT_INPUT_INPUT_PORT_HPP__
#define __PSXACT_INPUT_INPUT_PORT_HPP__

#include <cstdint>
#include "guest/input_device.hpp"

namespace psxact {
namespace input {

  struct port {
    guest::device *device;
    uint8_t step;
    bool active;

    void reset();

    bool send_request(uint8_t request, uint8_t *response);
  };

}
}

#endif // __PSXACT_INPUT_INPUT_PORT_HPP__
