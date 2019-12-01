// Copyright 2018 psxact

#ifndef INPUT_DEVICE_HPP_
#define INPUT_DEVICE_HPP_

#include <cstdint>

namespace psx {
namespace input {

// Represents the /ACK input, active-low.
enum class device_ack_t {
  HIGH = 0,
  LOW = 1
};

class device_t {
 private:
  device_ack_t ack;
  int ack_cycles;

 protected:
  void start_ack_sequence();

 public:
  device_ack_t tick(int amount);

  virtual void frame() = 0;
  virtual void reset() = 0;
  virtual void send(uint8_t request, uint8_t *response) = 0;
};

}  // namespace input
}  // namespace psx

#endif  // INPUT_DEVICE_HPP_
