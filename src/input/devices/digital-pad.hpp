// Copyright 2018 psxact

#ifndef INPUT_DEVICES_DIGITAL_PAD_HPP_
#define INPUT_DEVICES_DIGITAL_PAD_HPP_

#include <cstdint>
#include "input/device.hpp"

namespace psx {
namespace input {
namespace devices {

class digital_pad_t : public device_t {
  int bit = 0;
  int step = 0;
  uint16_t bits = 0;
  uint8_t rx_buffer = 0;
  uint8_t tx_buffer = 0xff;

 public:
  void frame();
  int send(int request);
  void set_dtr(bool next_dtr);
};

}  // namespace devices
}  // namespace input
}  // namespace psx

#endif  // INPUT_DEVICES_DIGITAL_PAD_HPP_
