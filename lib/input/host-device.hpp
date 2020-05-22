#ifndef INPUT_HOST_DEVICE_HPP_
#define INPUT_HOST_DEVICE_HPP_

namespace psx::input {

  enum class host_device_button {
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

  struct host_device {
    host_device_button dpad_down;
    host_device_button dpad_left;
    host_device_button dpad_right;
    host_device_button dpad_up;
    host_device_button left_back_shoulder;
    host_device_button left_front_shoulder;
    host_device_button right_back_shoulder;
    host_device_button right_front_shoulder;
    host_device_button select;
    host_device_button start;
    host_device_button button_0;
    host_device_button button_1;
    host_device_button button_2;
    host_device_button button_3;
  };
}

#endif
