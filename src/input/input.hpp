#ifndef __psxact_input__
#define __psxact_input__


#include "console.hpp"
#include "fifo.hpp"


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


class input_t {

  int32_t baud_factor;
  int32_t baud_reload;
  int32_t baud_timer;
  int32_t baud_elapses;

  bool irq;

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

  input_t();

  uint32_t io_read(bus_width_t width, uint32_t address);

  void io_write(bus_width_t width, uint32_t address, uint32_t data);

  void tick();

  void reload_baud();

private:
  input_port_t *get_selected_port();

  bool send(uint8_t request, uint8_t *response);

  bool send_null(input_port_t *port, uint8_t request, uint8_t *response);

  bool send_memory_card(input_port_t *port, uint8_t request, uint8_t *response);

  bool send_controller(input_port_t *port, uint8_t request, uint8_t *response);

  bool send_controller_digital(input_port_t *port, uint8_t request, uint8_t *response);

};


#endif // __psxact_input__
