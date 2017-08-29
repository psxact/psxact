#ifndef __PSXACT_INPUT_GUEST_DEVICE_HPP__
#define __PSXACT_INPUT_GUEST_DEVICE_HPP__

#include <cstdint>

namespace psxact {
namespace input {
namespace guest {

  enum button_t {
    SELECT = 0,
    START = 3,
    DPAD_UP = 4,
    DPAD_RIGHT = 5,
    DPAD_DOWN = 6,
    DPAD_LEFT = 7,
    L2 = 8,
    R2 = 9,
    L1 = 10,
    R1 = 11,
    TRIANGLE = 12,
    CIRCLE = 13,
    CROSS = 14,
    SQUARE = 15,
  };

  struct device {
    virtual bool send_request(int step, uint8_t request, uint8_t *response) = 0;

    virtual void press(button_t button) = 0;

    virtual void release(button_t button) = 0;
  };

}
}
}

#endif // __PSXACT_INPUT_GUEST_DEVICE_HPP__
