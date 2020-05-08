#include "cdrom/core.hpp"

#include <cassert>

using namespace psx::cdrom;

int core_t::dma_speed() {
  return 40;
}

bool core_t::dma_read_ready() {
  return true;
}

bool core_t::dma_write_ready() {
  return true;
}

uint32_t core_t::dma_read() {
  return
    (io_read(address_width_t::byte, 0x1f801802) << (8 * 0)) |
    (io_read(address_width_t::byte, 0x1f801802) << (8 * 1)) |
    (io_read(address_width_t::byte, 0x1f801802) << (8 * 2)) |
    (io_read(address_width_t::byte, 0x1f801802) << (8 * 3));
}

void core_t::dma_write(uint32_t) {
}

uint32_t core_t::io_read(address_width_t width, uint32_t address) {
  if (width == address_width_t::byte) {
    switch (address) {
      case 0x1f801800:
        return uint8_t(
          (index                     << 0) |
      //  (xa_adpcm.has_data()       << 2) |
          (parameter_fifo.is_empty() << 3) |
          (!parameter_fifo.is_full() << 4) |
          (!response_fifo.is_empty() << 5) |
          ((rx_index < rx_len)       << 6) |
          (busy                      << 7));

      case 0x1f801801:
        if (response_fifo.is_empty()) {
          log("!!! RESPONSE FIFO EMPTY !!!");
        }

        return response_fifo.read();

      case 0x1f801802: {
        auto data = rx_buffer[rx_index];

        if (rx_active) {
          rx_index += 1;

          if (rx_index == rx_len) {
            rx_active = false;
          }
        }

        return data;
      }

      case 0x1f801803:
        switch (index & 1) {
          case 0: return uint8_t(0xe0 | interrupt_enable);
          case 1: return uint8_t(0xe0 | interrupt_request);
        }
        break;
    }
  }

  return addressable_t::io_read(width, address);
}

void core_t::io_write_port_0_n(uint8_t data) {
  index = data & 3;
}

void core_t::io_write_port_1_0(uint8_t data) {
  command = data;
  command_unprocessed = 1;
}

void core_t::io_write_port_1_1(uint8_t) {}

void core_t::io_write_port_1_2(uint8_t) {}

void core_t::io_write_port_1_3(uint8_t) {}

void core_t::io_write_port_2_0(uint8_t data) {
  parameter_fifo.write(data);
}

void core_t::io_write_port_2_1(uint8_t data) {
  int32_t flags = data & 0x1f;
  interrupt_enable = flags;
}

void core_t::io_write_port_2_2(uint8_t) {}

void core_t::io_write_port_2_3(uint8_t) {}

void core_t::io_write_port_3_0(uint8_t data) {
  auto prev_active = rx_active;
  rx_active = (data & 0x80) != 0;

  if (rx_active && !prev_active) {
    rx_index = mode.read_whole_sector ? 12 : 24;
  }
}

void core_t::io_write_port_3_1(uint8_t data) {
  int32_t flags = data & 0x1f;
  interrupt_request &= ~flags;

  if (data & 0x40) {
    parameter_fifo.clear();
  }
}

void core_t::io_write_port_3_2(uint8_t) {}

void core_t::io_write_port_3_3(uint8_t) {}

void core_t::io_write(address_width_t width, uint32_t address, uint32_t data) {
  if (width == address_width_t::byte) {
    log("io_write_byte(0x%08x, 0x%08x)", address, data);

    switch (address) {
      case 0x1f801800:
        return io_write_port_0_n(data);

      case 0x1f801801:
        switch (index) {
          case 0: return io_write_port_1_0(data);
          case 1: return io_write_port_1_1(data);
          case 2: return io_write_port_1_2(data);
          case 3: return io_write_port_1_3(data);
        }
        break;

      case 0x1f801802:
        switch (index) {
          case 0: return io_write_port_2_0(data);
          case 1: return io_write_port_2_1(data);
          case 2: return io_write_port_2_2(data);
          case 3: return io_write_port_2_3(data);
        }
        break;

      case 0x1f801803:
        switch (index) {
          case 0: return io_write_port_3_0(data);
          case 1: return io_write_port_3_1(data);
          case 2: return io_write_port_3_2(data);
          case 3: return io_write_port_3_3(data);
        }
        break;
    }
  }

  return addressable_t::io_write(width, address, data);
}
