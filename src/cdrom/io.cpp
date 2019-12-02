// Copyright 2018 psxact

#include "cdrom/core.hpp"

#include <cassert>
#include "utility.hpp"

using psx::cdrom::core_t;

uint8_t core_t::io_read_byte(uint32_t address) {
  switch (address) {
    case 0x1f801800:
      return uint8_t(
        (index                     << 0) |
    //  (xa_adpcm.has_data()       << 2) |
        (parameter_fifo.is_empty() << 3) |
        (parameter_fifo.has_room() << 4) |
        (response_fifo.has_data()  << 5) |
        (data_fifo.has_data()      << 6) |
        (busy                      << 7));

    case 0x1f801801:
      if (response_fifo.is_empty()) {
        log("!!! RESPONSE FIFO EMPTY !!!");
      }

      return response_fifo.read();

    case 0x1f801802:
      if (data_fifo.is_empty()) {
        log("!!! DATA FIFO EMPTY !!!");
      }

      return data_fifo.read();

    case 0x1f801803:
      switch (index & 1) {
        case 0: return uint8_t(0xe0 | interrupt_enable);
        case 1: return uint8_t(0xe0 | interrupt_request);
      }
      break;
  }

  return memory_component_t::io_read_byte(address);
}

uint32_t core_t::io_read_word(uint32_t address) {
  return
    (io_read_byte(address) << (8 * 0)) |
    (io_read_byte(address) << (8 * 1)) |
    (io_read_byte(address) << (8 * 2)) |
    (io_read_byte(address) << (8 * 3));
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
  data_fifo.clear();

  if (data & 0x80) {
    int skip = mode.read_whole_sector ? 12 : 24;
    int size = mode.read_whole_sector ? 0x924 : 0x800;

    for (int i = 0; i < size; i++) {
      data_fifo.write(data_buffer[i + skip]);
    }
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

void core_t::io_write_byte(uint32_t address, uint8_t data) {
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

  return memory_component_t::io_write_byte(address, data);
}
