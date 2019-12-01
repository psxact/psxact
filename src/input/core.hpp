// Copyright 2018 psxact

#ifndef INPUT_CORE_HPP_
#define INPUT_CORE_HPP_

#include "input/device.hpp"
#include "console.hpp"
#include "fifo.hpp"
#include "interrupt-access.hpp"
#include "memory-component.hpp"

namespace psx {
namespace input {

struct port_t {
  device_t *memcard;
  device_t *control;
  device_t *selected;
  int output;
};

class core_t : public memory_component_t {

  fifo_t< uint8_t, 8 > rx;

  device_ack_t ack;
  int ack_interrupt_enable;
  int baud_counter;
  int baud_elapses;
  int baud_factor;
  int baud_reload;
  int rx_enable;
  int rx_interrupt_mode;
  int rx_interrupt_enable;
  int tx_data;
  int tx_data_pending;
  int tx_enable;
  int tx_interrupt_enable;
  int port_output;
  int port_select;
  int interrupt;

  port_t ports[2];
  interrupt_access_t *irq;

 public:
  explicit core_t(interrupt_access_t *irq, bool log_enabled);

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

}  // namespace input
}  // namespace psx

#endif  // INPUT_CORE_HPP_
