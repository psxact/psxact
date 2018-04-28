#include <cassert>
#include "cdrom/cdrom.hpp"
#include "utility.hpp"


static uint32_t get_port(uint32_t address) {
  return address - 0x1f801800;
}


uint8_t cdrom_t::io_read_port_0() {
  return uint8_t(
    (index                     << 0) |
//  (xa_adpcm.has_data()       << 2) |
    (parameter_fifo.is_empty() << 3) |
    (parameter_fifo.has_room() << 4) |
    (response_fifo.has_data()  << 5) |
    (data_fifo.has_data()      << 6) |
    (busy                      << 7)
  );
}


uint8_t cdrom_t::io_read_port_1() {
  return response_fifo.read();
}


uint8_t cdrom_t::io_read_port_2() {
  return data_fifo.read();
}


uint8_t cdrom_t::io_read_port_3() {
  switch (index & 1) {
    case 0: return uint8_t(0xe0 | interrupt_enable);
    case 1: return uint8_t(0xe0 | interrupt_request);
  }

  return 0;
}


uint8_t cdrom_t::io_read_internal(uint32_t port) {
  switch (port) {
  case 0:
    return io_read_port_0();

  case 1:
    return io_read_port_1();

  case 2:
    return io_read_port_2();

  case 3:
    return io_read_port_3();

  default:
    return 0;
  }
}


uint32_t cdrom_t::io_read(memory_size_t size, uint32_t address) {
  if (size == memory_size_t::word && address == 0x1f801800) {
    uint8_t b0 = data_fifo.read();
    uint8_t b1 = data_fifo.read();
    uint8_t b2 = data_fifo.read();
    uint8_t b3 = data_fifo.read();

    return (b0 << 0) | (b1 << 8) | (b2 << 16) | (b3 << 24);
  }

  assert(size == memory_size_t::byte);

  switch (get_port(address)) {
    case 0: return io_read_port_0();
    case 1: return io_read_port_1();
    case 2: return io_read_port_2();
    case 3: return io_read_port_3();
  }

  return 0;
}


void cdrom_t::io_write_port_0_n(uint8_t data) {
  index = data & 3;
}


void cdrom_t::io_write_port_1_0(uint8_t data) {
  command = data;
  command_is_new = 1;
}


void cdrom_t::io_write_port_1_1(uint8_t data) {}


void cdrom_t::io_write_port_1_2(uint8_t data) {}


void cdrom_t::io_write_port_1_3(uint8_t data) {}


void cdrom_t::io_write_port_2_0(uint8_t data) {
  parameter_fifo.write(data);
}


void cdrom_t::io_write_port_2_1(uint8_t data) {
  int32_t flags = data & 0x1f;
  interrupt_enable = flags;
}


void cdrom_t::io_write_port_2_2(uint8_t data) {}


void cdrom_t::io_write_port_2_3(uint8_t data) {}


void cdrom_t::io_write_port_3_0(uint8_t data) {
  data_fifo.clear();

  if (data & 0x80) {
    int skip = mode.read_whole_sector ? 12 : 24;
    int size = mode.read_whole_sector ? 0x924 : 0x800;

    for (int i = 0; i < size; i++) {
      data_fifo.write(data_buffer[i + skip]);
    }
  }
}


void cdrom_t::io_write_port_3_1(uint8_t data) {
  int32_t flags = data & 0x1f;
  interrupt_request &= ~flags;

  if (data & 0x40) {
    parameter_fifo.clear();
  }
}


void cdrom_t::io_write_port_3_2(uint8_t data) {}


void cdrom_t::io_write_port_3_3(uint8_t data) {}


void cdrom_t::io_write(memory_size_t size, uint32_t address, uint32_t data) {
  assert(size == memory_size_t::byte);

  uint32_t port = get_port(address);

  if (utility::log_cdrom) {
    printf("cdrom_t::io_write_port_%d_%d(0x%02x)\n", port, index, data);
  }

  uint8_t clipped = uint8_t(data);

  switch (port) {
    case 0:
      return io_write_port_0_n(data);

    case 1:
      switch (index) {
        case 0: return io_write_port_1_0(data);
        case 1: return io_write_port_1_1(data);
        case 2: return io_write_port_1_2(data);
        case 3: return io_write_port_1_3(data);
      }
      break;

    case 2:
      switch (index) {
        case 0: return io_write_port_2_0(data);
        case 1: return io_write_port_2_1(data);
        case 2: return io_write_port_2_2(data);
        case 3: return io_write_port_2_3(data);
      }
      break;

    case 3:
      switch (index) {
        case 0: return io_write_port_3_0(data);
        case 1: return io_write_port_3_1(data);
        case 2: return io_write_port_3_2(data);
        case 3: return io_write_port_3_3(data);
      }
      break;
    }
}
