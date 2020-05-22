#include "input/core.hpp"

#include "input/devices/digital-pad.hpp"
#include "args.hpp"
#include "timing.hpp"

using namespace psx::input;

core::core(interruptible &irq)
  : addressable("input", args::log_input)
  , irq(irq) {
  port.memcard[0] = &device::not_connected;
  port.control[0] = new devices::digital_pad();
  port.memcard[1] = &device::not_connected;
  port.control[1] = &device::not_connected;
}

void core::latch(const host_device &device1, const host_device &device2) {
  port.memcard[0]->latch(device1);
  port.memcard[1]->latch(device2);
  port.control[0]->latch(device1);
  port.control[1]->latch(device2);
}

void core::tick(int amount) {
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

  dsr.level = port.control[0]->tick(amount, device_dsr::high);
  dsr.level = port.memcard[0]->tick(amount, dsr.level);
  dsr.level = port.control[1]->tick(amount, dsr.level);
  dsr.level = port.memcard[1]->tick(amount, dsr.level);

  if (dsr.level == device_dsr::low) {
    send_interrupt();
  }
}

uint32_t core::io_read(address_width width, uint32_t address) {
  timing::add_cpu_time(4);

  if (width == address_width::byte && address == 0x1f801040) {
    uint8_t data = rx.fifo.is_empty() ? 0xff : rx.fifo.read();

    log("1040: rx '%02x'", data);

    return data;
  }

  if (width == address_width::word || width == address_width::half) {
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
  }

  return addressable::io_read(width, address);
}

void core::io_write(address_width width, uint32_t address, uint32_t data) {
  timing::add_cpu_time(4);

  if (width == address_width::byte && address == 0x1f801040) {
    tx.buffer = data & 0xff;
    tx.pending = true;

    log("1040: tx '%02x'", tx.buffer);
    return;
  }

  if (width == address_width::word || width == address_width::half) {
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
        } else {
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
  }

  return addressable::io_write(width, address, data);
}

void core::write_rx(uint8_t data) {
  rx.fifo.write(data);

  if (rx.interrupt_enable) {
    send_interrupt();
  }
}

void core::send_interrupt() {
  if (interrupt == 0) {
    log("sending interrupt");

    interrupt = 1;
    irq.interrupt(interrupt_type::input);
  }
}
