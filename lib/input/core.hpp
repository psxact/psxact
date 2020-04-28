#ifndef INPUT_CORE_HPP_
#define INPUT_CORE_HPP_

#include "input/device.hpp"
#include "util/fifo.hpp"
#include "addressable.hpp"
#include "interruptible.hpp"

using namespace psx::util;

namespace psx::input {

class core_t final : public addressable_t {

  struct {
    int counter = 0x0088;
    int factor = 1;
    int reload = 0x0088;
  } baud = {};

  struct {
    device_dsr_t level = {};
    int interrupt_enable = {};
  } dsr = {};

  struct {
    fifo_t< uint8_t, 3 > fifo = {};
    bool enable = {};
    bool interrupt_enable = {};
    int interrupt_mode = {};
    uint8_t buffer = {};
  } rx = {};

  struct {
    uint8_t buffer = {};
    bool enable = {};
    bool pending = {};
    bool interrupt_enable = {};
  } tx = {};

  struct {
    device_t *memcard[2] = {};
    device_t *control[2] = {};
    int output = {};
    int select = {};
  } port = {};

  int bit = {};
  int interrupt = {};

  interruptible_t &irq;

 public:
  explicit core_t(interruptible_t &irq, bool log_enabled);

  void frame();

  void tick(int amount);

  uint8_t io_read_byte(uint32_t address);

  uint16_t io_read_half(uint32_t address);

  uint32_t io_read_word(uint32_t address);

  void io_write_byte(uint32_t address, uint8_t data);

  void io_write_half(uint32_t address, uint16_t data);

  void io_write_word(uint32_t address, uint32_t data);

 private:
  void write_rx(uint8_t data);

  void send_interrupt();
};

}  // namespace psx::input

#endif  // INPUT_CORE_HPP_
