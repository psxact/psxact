// Copyright 2018 psxact

#ifndef INPUT_DEVICES_NOT_CONNECTED_HPP_
#define INPUT_DEVICES_NOT_CONNECTED_HPP_

#include <cstdint>
#include "input/device.hpp"

namespace psx {
namespace input {
namespace devices {

class not_connected_t : public device_t {
  not_connected_t();

 public:
  static const not_connected_t instance;

  void frame();
  void reset();
  void send(uint8_t request, uint8_t *response);
};

}  // namespace devices
}  // namespace input
}  // namespace psx

#endif  // INPUT_DEVICES_NOT_CONNECTED_HPP_
