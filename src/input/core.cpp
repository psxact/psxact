#include "input/core.hpp"

#include "input/devices/digital-pad.hpp"

using namespace psx::input;

core_t::core_t(interrupt_access_t *irq, bool log_enabled)
  : memory_component_t("input", log_enabled)
  , baud()
  , dsr()
  , rx()
  , tx()
  , port()
  , bit()
  , interrupt()
  , irq(irq) {
  port.memcard[0] = &device_t::not_connected;
  port.control[0] = new devices::digital_pad_t();
  port.memcard[1] = &device_t::not_connected;
  port.control[1] = &device_t::not_connected;
}

void core_t::frame() {
  for (device_t *memcard : port.memcard) {
    memcard->frame();
  }

  for (device_t *control : port.control) {
    control->frame();
  }
}

void core_t::tick(int amount) {
  baud.counter -= amount;

  while (baud.counter <= 0) {
    baud.counter += baud.reload * baud.factor;

    // clock output shifter

    if (tx.enable && tx.pending) {
      int tx_bit = (tx.buffer >> bit) & 1;
      tx.buffer |= (1 << bit);

      int rx_bit =
        port.control[0]->send(tx_bit) &
        port.memcard[0]->send(tx_bit) &
        port.control[1]->send(tx_bit) &
        port.memcard[1]->send(tx_bit);

      rx.buffer &= ~(1 << bit);
      rx.buffer |= rx_bit << bit;

      bit++;

      if (bit == 8) {
        bit = 0;
        tx.pending = false;

        write_rx(rx.buffer);
      }
    }
  }

  dsr.level = port.control[0]->tick(amount, device_dsr_t::HIGH);
  dsr.level = port.memcard[0]->tick(amount, dsr.level);
  dsr.level = port.control[1]->tick(amount, dsr.level);
  dsr.level = port.memcard[1]->tick(amount, dsr.level);

  if (dsr.level == device_dsr_t::LOW) {
    send_interrupt();
  }
}

uint8_t core_t::io_read_byte(uint32_t address) {
  switch (address) {
    case 0x1f801040:
      uint8_t data = rx.fifo.is_empty()
        ? 0xff
        : rx.fifo.read();

      log("1040: rx '%02x'", data);

      return data;
  }

  return memory_component_t::io_read_byte(address);
}

uint16_t core_t::io_read_half(uint32_t address) {
  switch (address) {
    case 0x1f801044: {
      uint16_t data =
        (tx.pending ? 0 : 1) | //   0     TX Ready Flag 1   (1=Ready/Started)
        (!rx.fifo.is_empty() << 1) |
        (1 << 2) | //   2     TX Ready Flag 2   (1=Ready/Finished)
        (0 << 3) | //   3     RX Parity Error   (0=No, 1=Error; Wrong Parity, when enabled)  (sticky)
        (int(dsr.level) << 7) |
        (interrupt << 9) |
        (baud.counter << 11);

      log("1044: returning '%04x'", data);

      return data;
    }

    case 0x1f80104a: {
      uint16_t data =
        (tx.enable << 0) |
        (port.output << 1) |
        (rx.enable << 2) |
        (rx.interrupt_mode << 8) |
        (tx.interrupt_enable << 10) |
        (rx.interrupt_enable << 11) |
        (dsr.interrupt_enable << 12) |
        (port.select << 13);

      log("104a: returning '%04x'", data);

      return data;
    }

    case 0x1f80104e:
      return baud.reload;
  }

  return memory_component_t::io_read_half(address);
}

uint32_t core_t::io_read_word(uint32_t address) {
  return memory_component_t::io_read_word(address);
}

void core_t::io_write_byte(uint32_t address, uint8_t data) {
  switch (address) {
    case 0x1f801040:
      tx.buffer = data & 0xff;
      tx.pending = true;

      log("1040: tx '%02x'", tx.buffer);
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
        log("1048 - non-standard value: %04x", data);
      }
      return;

    case 0x1f80104a:
      if (data & 0x40) {
        log("resetting input");
        interrupt = 0;
        port.output = 0;
        port.select = 0;
        rx.fifo.clear();
      }
      else {
        if (data & 0x10) {
          interrupt = 0;
        }

        tx.enable = (data >> 0) & 1;
        port.output = (data >> 1) & 1;
        rx.enable = (data >> 2) & 1;
        rx.interrupt_mode = (data >> 8) & 3;
        tx.interrupt_enable = (data >> 10) & 1;
        rx.interrupt_enable = (data >> 11) & 1;
        dsr.interrupt_enable = (data >> 12) & 1;
        port.select = (data >> 13) & 1;

        port.control[0]->set_dtr(port.output == 1 && port.select == 0);
        port.memcard[0]->set_dtr(port.output == 1 && port.select == 0);
        port.control[1]->set_dtr(port.output == 1 && port.select == 1);
        port.memcard[1]->set_dtr(port.output == 1 && port.select == 1);
      }
      return;

    case 0x1f80104e:
      baud.reload = data & 0xffff;
      return;
  }

  return memory_component_t::io_write_half(address, data);
}

void core_t::io_write_word(uint32_t address, uint32_t data) {
  return memory_component_t::io_write_word(address, data);
}

void core_t::write_rx(uint8_t data) {
  rx.fifo.write(data);

  if (rx.interrupt_enable) {
    send_interrupt();
  }
}

void core_t::send_interrupt() {
  if (interrupt == 0) {
    log("sending interrupt");

    interrupt = 1;
    irq->send(interrupt_type_t::INPUT);
  }
}
