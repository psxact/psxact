#ifndef INPUT_DEVICE_HPP_
#define INPUT_DEVICE_HPP_

#include <cstdint>

namespace psx::input {

enum class device_dsr_t {
  HIGH = 0,
  LOW = 1
};

class device_t {
 private:
  static const int DSR_DELAY_PERIOD = 100;
  static const int DSR_PULSE_PERIOD = 100;

  bool dsr_pending = false;
  int dsr_cycles = 0;

 protected:
  bool dtr = false;

  device_t() {}
  ~device_t() {}

  void start_dsr_pulse();

 public:
  static device_t not_connected;

  void tick(int amount, device_dsr_t &dsr);

  virtual void frame();
  virtual int send(int request);
  virtual void set_dtr(bool next_dtr);
};

}  // namespace psx::input

#endif  // INPUT_DEVICE_HPP_
