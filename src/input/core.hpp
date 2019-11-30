// Copyright 2018 psxact

#ifndef INPUT_CORE_HPP_
#define INPUT_CORE_HPP_

#include "console.hpp"
#include "fifo.hpp"
#include "interrupt-access.hpp"
#include "memory-component.hpp"

namespace psx {
namespace input {

class core_t : public memory_component_t {

  fifo_t< uint8_t, 8 > rx;

  int ack;
  int ack_interrupt_cycles;
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
  int slot_output;
  int slot_select;
  int interrupt;

  struct device_t {
    int output;
    int index;
    int lower;
    int upper;
  };

  device_t devices[2];
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
  void write_rx(uint8_t data, bool ack);
};

}  // namespace input
}  // namespace psx

#endif  // INPUT_CORE_HPP_
