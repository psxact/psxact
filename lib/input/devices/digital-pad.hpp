#ifndef INPUT_DEVICES_DIGITAL_PAD_HPP_
#define INPUT_DEVICES_DIGITAL_PAD_HPP_

#include <cstdint>
#include "input/device.hpp"

namespace psx::input::devices {

  class digital_pad : public device {
    int bit = {};
    int step = {};
    uint16_t bits = {};
    uint8_t rx_buffer = {};
    uint8_t tx_buffer = {0xff};

  public:
    void latch(const host_device &device) override;
    int send(int request) override;
    void set_dtr(bool next_dtr) override;
  };
}

#endif
