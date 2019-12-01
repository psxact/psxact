// Copyright 2018 psxact

#ifndef INPUT_DEVICES_DIGITAL_PAD_HPP_
#define INPUT_DEVICES_DIGITAL_PAD_HPP_

#include <cstdint>
#include "input/device.hpp"

namespace psx {
namespace input {
namespace devices {

class digital_pad_t : public device_t {
  int step;
  uint16_t bits;
  device_ack_t ack;
  int ack_cycles;

 public:
  void frame();
  void reset();
  void send(uint8_t request, uint8_t *response);
  device_ack_t tick(int amount);
};

}  // namespace devices
}  // namespace input
}  // namespace psx

#endif  // INPUT_DEVICES_DIGITAL_PAD_HPP_
