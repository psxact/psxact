#ifndef INPUT_CORE_HPP_
#define INPUT_CORE_HPP_

#include "input/device.hpp"
#include "input/host-device.hpp"
#include "util/fifo.hpp"
#include "addressable.hpp"
#include "interruptible.hpp"

namespace psx::input {

  class core final : public addressable {

    struct {
      int counter = 0x0088;
      int factor = 1;
      int reload = 0x0088;
    } baud = {};

    struct {
      device_dsr level = {};
      int interrupt_enable = {};
    } dsr = {};

    struct {
      util::fifo< uint8_t, 3 > fifo = {};
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
      device *memcard[2] = {};
      device *control[2] = {};
      int output = {};
      int select = {};
    } port = {};

    int bit = {};
    int interrupt = {};

    interruptible &irq;

  public:
    explicit core(opts &o, interruptible &irq);

    void latch(const host_device &device1, const host_device &device2);

    void tick(int amount);

    uint32_t io_read(address_width width, uint32_t address) override;
    void io_write(address_width width, uint32_t address, uint32_t data) override;

  private:
    void write_rx(uint8_t data);

    void send_interrupt();
  };
}

#endif
