// Copyright 2018 psxact

#ifndef INPUT_CORE_HPP_
#define INPUT_CORE_HPP_

#include "console.hpp"
#include "fifo.hpp"
#include "interrupt-access.hpp"
#include "memory-component.hpp"

namespace psx {
namespace input {

enum class input_port_status_t {
  none,
  selected
};

enum class input_port_access_t {
  controller = 0,
  memory_card = 1
};

struct input_port_t {
  input_port_access_t access;
  input_port_status_t status;
  int sequence;
  uint16_t data;
};

class core_t : public memory_component_t {
  interrupt_access_t *irq;

  int32_t baud_factor;
  int32_t baud_reload;
  int32_t baud_timer;
  int32_t baud_elapses;

  int32_t control;

  bool interrupt;

  bool dsr;
  int dsr_cycles;
  bool tx_enable;
  bool tx_occurring;
  bool rx_enable;
  bool rx_occurred;
  uint8_t tx_data;
  uint8_t rx_data;

  input_port_t ports[2];

 public:
  explicit core_t(interrupt_access_t *irq);

  uint32_t io_read_byte(uint32_t address);

  uint32_t io_read_half(uint32_t address);

  uint32_t io_read_word(uint32_t address);

  void io_write_byte(uint32_t address, uint32_t data);

  void io_write_half(uint32_t address, uint32_t data);

  void io_write_word(uint32_t address, uint32_t data);

  void tick(int amount);

  void reload_baud();

 private:
  input_port_t *get_selected_port();

  bool send(uint8_t request, uint8_t *response);

  bool send_null(input_port_t *port, uint8_t request, uint8_t *response);

  bool send_memory_card(input_port_t *port, uint8_t request, uint8_t *response);

  bool send_controller(input_port_t *port, uint8_t request, uint8_t *response);

  bool send_controller_digital(input_port_t *port, uint8_t request, uint8_t *response);
};

}  // namespace input
}  // namespace psx

#endif  // INPUT_CORE_HPP_
