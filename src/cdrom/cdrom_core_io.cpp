#include <cassert>
#include "cdrom_core.hpp"
#include "../utility.hpp"

using namespace psxact;
using namespace psxact::cdrom;

static uint32_t get_port(uint32_t address) {
  return address - 0x1f801800;
}

uint8_t core::io_read_port_0() {
  return uint8_t(
      (index) |
      //(xa_adpcm.has_data() << 2) |
      (parameter.is_empty() << 3) |
      (parameter.has_room() << 4) |
      (response.has_data() << 5) |
      (data.has_data() << 6) |
      (busy << 7)
  );
}

uint8_t core::io_read_port_1() {
  return response.read();
}

uint8_t core::io_read_port_2() {
  return data.read();
}

uint8_t core::io_read_port_3() {
  switch (index) {
  case 0:
  case 2:
    return uint8_t(0xe0 | interrupt_enable);

  case 1:
  case 3:
    return uint8_t(0xe0 | interrupt_request);

  default:
    return 0;
  }
}

uint8_t core::io_read_internal(uint32_t port) {
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

uint32_t core::io_read(bus_width_t width, uint32_t address) {
  if (width == BUS_WIDTH_WORD && address == 0x1f801800) {
    uint8_t b0 = data.read();
    uint8_t b1 = data.read();
    uint8_t b2 = data.read();
    uint8_t b3 = data.read();

    return (b0 << 0) | (b1 << 8) | (b2 << 16) | (b3 << 24);
  }

  assert(width == BUS_WIDTH_BYTE);

  uint32_t port = get_port(address);
  uint32_t data = io_read_internal(port);

  if (utility::log_cdrom) {
    printf("cdrom_core::io_read_port_%d_%d() returned 0x%02x\n", port, index, data);
  }

  return data;
}

void core::io_write_port_0_n(uint8_t data) {
  index = data & 3;
}

void core::io_write_port_1_0(uint8_t data) {
  command = data;
  command_is_new = 1;
}

void core::io_write_port_1_1(uint8_t data) {
}

void core::io_write_port_1_2(uint8_t data) {
}

void core::io_write_port_1_3(uint8_t data) {
}

void core::io_write_port_2_0(uint8_t data) {
  parameter.write(data);
}

void core::io_write_port_2_1(uint8_t data) {
  int32_t flags = data & 0x1f;
  interrupt_enable = flags;
}

void core::io_write_port_2_2(uint8_t data) {
}

void core::io_write_port_2_3(uint8_t data) {
}

void core::io_write_port_3_0(uint8_t data) {
  this->data.clear();

  if (data & 0x80) {
    int skip = mode.read_whole_sector ? 12 : 24;
    int size = mode.read_whole_sector ? 0x924 : 0x800;

    for (int i = 0; i < size; i++) {
      this->data.write(data_buffer[i + skip]);
    }
  }
}

void core::io_write_port_3_1(uint8_t data) {
  int32_t flags = data & 0x1f;
  interrupt_request &= ~flags;

  if (data & 0x40) {
    parameter.clear();
  }
}

void core::io_write_port_3_2(uint8_t data) {
}

void core::io_write_port_3_3(uint8_t data) {
}

void core::io_write(bus_width_t width, uint32_t address, uint32_t data) {
  assert(width == BUS_WIDTH_BYTE);

  uint32_t port = get_port(address);

  if (utility::log_cdrom) {
    printf("cdrom_core::io_write_port_%d_%d(0x%02x)\n", port, index, data);
  }

  uint8_t clipped = uint8_t(data);

  switch (port) {
  case 0:
    return io_write_port_0_n(clipped);

  case 1:
    switch (index) {
    case 0:
      return io_write_port_1_0(clipped);

    case 1:
      return io_write_port_1_1(clipped);

    case 2:
      return io_write_port_1_2(clipped);

    case 3:
      return io_write_port_1_3(clipped);
    }

  case 2:
    switch (index) {
    case 0:
      return io_write_port_2_0(clipped);

    case 1:
      return io_write_port_2_1(clipped);

    case 2:
      return io_write_port_2_2(clipped);

    case 3:
      return io_write_port_2_3(clipped);
    }

  case 3:
    switch (index) {
    case 0:
      return io_write_port_3_0(clipped);

    case 1:
      return io_write_port_3_1(clipped);

    case 2:
      return io_write_port_3_2(clipped);

    case 3:
      return io_write_port_3_3(clipped);
    }
  }
}
