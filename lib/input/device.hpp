#ifndef INPUT_DEVICE_HPP_
#define INPUT_DEVICE_HPP_

#include <cstdint>
#include "input/host-device.hpp"

namespace psx::input {

  enum class device_dsr_t {
    high = 0,
    low = 1
  };

  class device_t {
  private:
    static const int DSR_DELAY_PERIOD = 100;
    static const int DSR_PULSE_PERIOD = 100;

    bool dsr_pending = {};
    int dsr_cycles = {};

  protected:
    bool dtr = {};

    device_t() {}
    ~device_t() {}

    void start_dsr_pulse();

  public:
    static device_t not_connected;

    device_dsr_t tick(int amount, device_dsr_t dsr);

    virtual void latch(const host_device_t &device);
    virtual int send(int request);
    virtual void set_dtr(bool next_dtr);
  };
}

#endif
