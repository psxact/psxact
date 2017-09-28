#ifndef __PSXACT_INPUT_HPP__
#define __PSXACT_INPUT_HPP__

#include "../system_core.hpp"
#include "../fifo.hpp"

namespace psxact {
namespace input {

  enum class port_status {
    none,
    selected
  };

  enum class port_access {
    controller = 0,
    memory_card = 1
  };

  struct port {
    port_access access;
    port_status status;
    int sequence;
    uint16_t data;
  };

  struct core {
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

    port ports[2];

    core();

    uint32_t io_read(bus_width_t width, uint32_t address);

    void io_write(bus_width_t width, uint32_t address, uint32_t data);

    void tick();

    void reload_baud();

  private:
    port *get_selected_port();

    bool send(uint8_t request, uint8_t *response);

    bool send_null(port *port, uint8_t request, uint8_t *response);

    bool send_memory_card(port *port, uint8_t request, uint8_t *response);

    bool send_controller(port *port, uint8_t request, uint8_t *response);

    bool send_controller_digital(port *port, uint8_t request, uint8_t *response);
  };

}
}

#endif // __PSXACT_INPUT_HPP__
