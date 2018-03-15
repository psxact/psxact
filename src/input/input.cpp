#include "input/input.hpp"
#include "utility.hpp"


input_t::input_t() {
  baud_factor = 1;
  baud_reload = 0x0088;

  reload_baud();
}


static uint32_t get_index(uint32_t address) {
  return address - 0x1f801040;
}


uint32_t input_t::io_read(bus_width_t width, uint32_t address) {
  if (utility::log_input) {
    printf("input::io_read(%d, 0x%08x)\n", width, address);
  }

  switch (get_index(address)) {
  case 0:
    if (rx_occurred) {
      rx_occurred = 0;
      return 0xffffff00 | rx_data;
    }
    else {
      return 0xffffffff;
    }

  case 4:
    return
        (1           << 0) | // tx_ready_1
        (rx_occurred << 1) |
        (1           << 2) | // tx_ready_2
        (0           << 3) | // rx_parity_error
        (dsr         << 7) |
        (irq         << 9) |
        (baud_timer  << 11);
  }

  return 0;
}


static int32_t get_baud_rate_factor(uint32_t data) {
  static const int lut[4] = {1, 1, 16, 64};

  return lut[data & 3];
}


void input_t::io_write(bus_width_t width, uint32_t address, uint32_t data) {
  if (utility::log_input) {
    printf("input::io_write(%d, 0x%08x, 0x%08x)\n", width, address, data);
  }

  switch (get_index(address)) {
  case 0x0:
    tx_data = uint8_t(data);
    tx_occurring = tx_enable;
    baud_elapses = 0;
    break;

  case 0x8:
    baud_factor = get_baud_rate_factor(data);
    break;

  case 0xa:
    if (data & (1 << 4)) {
      irq = 0;
    }
    else {
      tx_enable = ((data >> 0) & 1) != 0;
      rx_enable = ((data >> 2) & 1) != 0;

      auto &port = ports[(data >> 13) & 1];

      if (data & (1 << 1)) {
        port.sequence = 0;
        port.status = input_port_status_t::selected;
      }
      else {
        port.status = input_port_status_t::none;
      }
    }
    break;

  case 0xe:
    baud_reload = data & 0xffff;
    break;
  }
}


void input_t::reload_baud() {
  baud_timer = baud_reload * baud_factor;
}


void input_t::tick() {
  if (dsr_cycles && !--dsr_cycles && dsr) {
    irq = 1;
    bus->irq(7);
  }

  baud_timer--;

  if (baud_timer == 0) {
    reload_baud();

    baud_elapses++;

    if (baud_elapses == 8) {
      baud_elapses = 0;

      if (tx_occurring) {
        tx_occurring = 0;
        rx_occurred = 1;

        dsr = send(tx_data, &rx_data);
        dsr_cycles = 100; // delay 100 cycles before issuing an interrupt, per the documentation.

        printf("input::transfer(0x%02x) = 0x%02x\n", tx_data, rx_data);
      }
    }
  }
}


input_port_t *input_t::get_selected_port() {
  if (ports[0].status == input_port_status_t::selected) {
    return &ports[0];
  }

  if (ports[1].status == input_port_status_t::selected) {
    return &ports[1];
  }

  return nullptr;
}


bool input_t::send(uint8_t request, uint8_t *response) {
  auto port = get_selected_port();
  if (port == nullptr) {
    return false;
  }

  if (port->sequence == 0) {
    if (tx_data & 0x80) {
      port->access = input_port_access_t::memory_card;
    }
    else {
      port->access = input_port_access_t::controller;
    }
  }

  switch (port->access) {
  case input_port_access_t::controller:
    return send_controller(port, request, response);

  case input_port_access_t::memory_card:
    return send_memory_card(port, request, response);

  default:
    return send_null(port, request, response);
  }
}


bool input_t::send_controller(struct input_port_t *port, uint8_t request, uint8_t *response) {
  return send_controller_digital(port, request, response);
}


bool input_t::send_memory_card(struct input_port_t *port, uint8_t request, uint8_t *response) {
  return send_null(port, request, response);
}


bool input_t::send_null(struct input_port_t *port, uint8_t request, uint8_t *response) {
  *response = 0xff;
  return false;
}


bool input_t::send_controller_digital(struct input_port_t *port, uint8_t request, uint8_t *response) {
  auto sequence = port->sequence;
  port->sequence++;

  switch (sequence) {
  case 0:
    *response = 0xff;
    return request == 0x01;

  case 1:
    *response = 0x41;
    return request == 0x42;

  case 2:
    *response = 0x5a;
    return 1;

  case 3:
    *response = port->data >> 0;
    return 1;

  case 4:
    *response = port->data >> 8;
    return 0;

  default:
    return send_null(port, request, response);
  }
}
