// Copyright 2018 psxact

#include "input/core.hpp"

#include <SDL.h>

#include "input/devices/digital-pad.hpp"
#include "input/devices/not-connected.hpp"
#include "utility.hpp"

using namespace psx::input;
using psx::input::core_t;

core_t::core_t(interrupt_access_t *irq, bool log_enabled)
  : memory_component_t("input", log_enabled)
  , ports()
  , irq(irq) {
  baud_factor = 1;
  baud_reload = 0x0088;
  baud_counter = baud_reload * baud_factor;

  device_t *not_connected = (device_t *) &devices::not_connected_t::instance;

  ports[0].memcard = not_connected;
  ports[0].control = new devices::digital_pad_t();
  ports[1].memcard = not_connected;
  ports[1].control = not_connected;
}

void core_t::frame() {
  for (port_t &port : ports) {
    port.control->frame();
    port.memcard->frame();
  }
}

void core_t::tick(int amount) {
  port_t &port = ports[port_select];

  baud_counter -= amount;

  while (baud_counter <= 0) {
    baud_counter += baud_reload * baud_factor;
    baud_elapses++;

    if (baud_elapses == 8) {
      baud_elapses = 0;

      if (tx_data_pending) {
        tx_data_pending = 0;

        if (port.selected == nullptr) {
          if (tx_data & 0x80) {
            log("selecting memory card on port %d", port_select);
            port.selected = port.memcard;
          }
          else {
            log("selecting controller on port %d", port_select);
            port.selected = port.control;
          }

          port.selected->reset();
        }

        uint8_t rx_data;
        port.selected->send(tx_data, &rx_data);

        log("sent '%02x' to slot %d, received '%02x' (ack: %d)", tx_data, port_select, rx_data, ack);

        write_rx(rx_data);
      }
    }
  }

  if (port.selected) {
    device_ack_t next_ack = port.selected->tick(amount);

    if (ack_interrupt_enable) {
      if (ack == device_ack_t::LOW && next_ack == device_ack_t::HIGH) {
        send_interrupt();
      }
    }

    ack = next_ack;
  }
}

uint8_t core_t::io_read_byte(uint32_t address) {
  switch (address) {
    case 0x1f801040:
      uint8_t data = rx.has_data()
        ? rx.read()
        : 0xff;

      log("1040: returning '%02x'", data);

      return data;
  }

  return memory_component_t::io_read_byte(address);
}

uint16_t core_t::io_read_half(uint32_t address) {
  switch (address) {
    case 0x1f801044: {
      uint16_t data =
        (1 << 0) | //   0     TX Ready Flag 1   (1=Ready/Started)
        (rx.has_data() << 1) |
        (1 << 2) | //   2     TX Ready Flag 2   (1=Ready/Finished)
        (0 << 3) | //   3     RX Parity Error   (0=No, 1=Error; Wrong Parity, when enabled)  (sticky)
        (int(ack) << 7) |
        (interrupt << 9) |
        (baud_counter << 11);

      log("1044: returning '%04x'", data);

      return data;
    }

    case 0x1f80104a: {
      uint16_t data =
        (tx_enable << 0) |
        (port_output << 1) |
        (rx_enable << 2) |
        (rx_interrupt_mode << 8) |
        (tx_interrupt_enable << 10) |
        (rx_interrupt_enable << 11) |
        (ack_interrupt_enable << 12) |
        (port_select << 13);

      log("104a: returning '%04x'", data);

      return data;
    }
  }

  return memory_component_t::io_read_half(address);
}

uint32_t core_t::io_read_word(uint32_t address) {
  return memory_component_t::io_read_word(address);
}

void core_t::io_write_byte(uint32_t address, uint8_t data) {
  switch (address) {
    case 0x1f801040:
      tx_data = data & 0xff;
      tx_data_pending = 1;
      baud_elapses = 0;
      return;
  }

  return memory_component_t::io_write_byte(address, data);
}

void core_t::io_write_half(uint32_t address, uint16_t data) {
  switch (address) {
    case 0x1f801048:
      // 1F801048h JOY_MODE (R/W) (usually 000Dh, ie. 8bit, no parity, MUL1)
      // 
      //   0-1   Baudrate Reload Factor (1=MUL1, 2=MUL16, 3=MUL64) (or 0=MUL1, too)
      //   2-3   Character Length       (0=5bits, 1=6bits, 2=7bits, 3=8bits)
      //   4     Parity Enable          (0=No, 1=Enable)
      //   5     Parity Type            (0=Even, 1=Odd) (seems to be vice-versa...?)
      //   6-7   Unknown (always zero)
      //   8     CLK Output Polarity    (0=Normal:High=Idle, 1=Inverse:Low=Idle)
      //   9-15  Unknown (always zero)

      if (data != 0x000d) {
        log("non-standard value for JOY_MODE: %04x", data);
      }
      return;

    case 0x1f80104a:
      if (data & 0x40) {
        log("resetting input");
        interrupt = 0;
        port_output = 0;
        port_select = 0;
        rx.clear();
      }
      else {
        if (data & 0x10) {
          interrupt = 0;
        }

        tx_enable = (data >> 0) & 1;
        port_output = (data >> 1) & 1;
        rx_enable = (data >> 2) & 1;
        rx_interrupt_mode = (data >> 8) & 3;
        tx_interrupt_enable = (data >> 10) & 1;
        rx_interrupt_enable = (data >> 11) & 1;
        ack_interrupt_enable = (data >> 12) & 1;
        port_select = (data >> 13) & 1;

        if (ports[port_select].output < port_output) {
          log("resetting slot %d", port_select);

          ports[port_select].selected = nullptr;
        }

        ports[port_select].output = port_output;
      }
      return;

    case 0x1f80104e:
      baud_reload = data & 0xffff;
      return;
  }

  return memory_component_t::io_write_half(address, data);
}

void core_t::io_write_word(uint32_t address, uint32_t data) {
  return memory_component_t::io_write_word(address, data);
}

void core_t::write_rx(uint8_t data) {
  rx.write(data);

  if (rx_interrupt_enable) {
    send_interrupt();
  }
}

void core_t::send_interrupt() {
    log("sending interrupt");

    interrupt = 1;
    irq->send(interrupt_type_t::INPUT);
}
