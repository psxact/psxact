#ifndef INPUT_HOST_DEVICE_HPP_
#define INPUT_HOST_DEVICE_HPP_

namespace psx::input {

  enum class host_device_button_t {
    released = 0,
    pressed = 1
  };

  /**
    *   .----.                .----.
    *   | LB |________________| RB |
    *   | LF \________________/ RF |
    *  /                            \
    * /   U   [SELECT] [START]   3   \
    * |  L R  ________________  1 2  |
    *  \  D  /                \  0  /
    *   |   |                  |   |
    *    \_/                    \_/
    */

  struct host_device_t {
    host_device_button_t dpad_down;
    host_device_button_t dpad_left;
    host_device_button_t dpad_right;
    host_device_button_t dpad_up;
    host_device_button_t left_back_shoulder;
    host_device_button_t left_front_shoulder;
    host_device_button_t right_back_shoulder;
    host_device_button_t right_front_shoulder;
    host_device_button_t select;
    host_device_button_t start;
    host_device_button_t button_0;
    host_device_button_t button_1;
    host_device_button_t button_2;
    host_device_button_t button_3;
  };
}  // namespace psx::input

#endif  // INPUT_DEVICE_HPP_
