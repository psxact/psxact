#ifndef INPUT_DEVICE_HPP_
#define INPUT_DEVICE_HPP_

#include <cstdint>
#include "input/host-device.hpp"

namespace psx::input {

  enum class device_dsr {
    high = 0,
    low = 1
  };

  class device {
  private:
    static const int DSR_DELAY_PERIOD = 100;
    static const int DSR_PULSE_PERIOD = 100;

    bool dsr_pending = {};
    int dsr_cycles = {};

  protected:
    bool dtr = {};

    device() {}
    ~device() {}

    void start_dsr_pulse();

  public:
    static device not_connected;

    device_dsr tick(int amount, device_dsr dsr);

    virtual void latch(const host_device &device);
    virtual int send(int request);
    virtual void set_dtr(bool next_dtr);
  };
}

#endif
