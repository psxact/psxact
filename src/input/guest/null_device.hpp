#ifndef __PSXACT_INPUT_GUEST_NULL_DEVICE_HPP__
#define __PSXACT_INPUT_GUEST_NULL_DEVICE_HPP__

#include "input_device.hpp"

namespace psxact {
namespace input {
namespace guest {

  struct null_device : public device {
    bool send_request(int step, uint8_t request, uint8_t *response);

    void press(button_t button);

    void release(button_t button);
  };

}
}
}

#endif // __PSXACT_INPUT_GUEST_NULL_DEVICE_HPP__
