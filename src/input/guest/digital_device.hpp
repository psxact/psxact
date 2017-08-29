#ifndef __PSXACT_INPUT_GUEST_DIGITAL_DEVICE_HPP__
#define __PSXACT_INPUT_GUEST_DIGITAL_DEVICE_HPP__

#include "input_device.hpp"

namespace psxact {
namespace input {
namespace guest {

  struct digital_device : public device {
    uint16_t data;

    digital_device();

    bool send_request(int step, uint8_t request, uint8_t *response);

    void press(button_t button);

    void release(button_t button);
  };

}
}
}

#endif // __PSXACT_INPUT_GUEST_DIGITAL_DEVICE_HPP__
