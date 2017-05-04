#include <cassert>
#include "cdrom_core.hpp"
#include "../bus.hpp"

cdrom::state_t cdrom::state;

static uint8_t read_arg_byte() {
  auto parameter = cdrom::state.args_fifo.front();
  cdrom::state.args_fifo.pop_front();

  return parameter;
}

static void write_arg_byte(uint8_t data) {
  cdrom::state.args_fifo.push_back(data);
}

static uint8_t read_resp_byte() {
  auto response = cdrom::state.resp_fifo.front();
  cdrom::state.resp_fifo.pop_front();

  return response;
}

static void write_resp_byte(uint8_t data) {
  cdrom::state.resp_fifo.push_back(data);
}

static uint8_t read_data_byte() {
  auto data = cdrom::state.data_fifo.front();
  cdrom::state.data_fifo.pop_front();

  return data;
}

static void write_data_byte(uint8_t data) {
  cdrom::state.data_fifo.push_back(data);
}

uint32_t cdrom::bus_read(int width, uint32_t address) {
  assert(width == bus::BUS_WIDTH_BYTE);

  switch (address - 0x1f801800) {
    case 0: { // status register
      uint32_t result = state.index;

      if (state.args_fifo.size() ==  0) { result |= 1 << 3; }
      if (state.args_fifo.size() != 16) { result |= 1 << 4; }
      if (state.resp_fifo.size() !=  0) { result |= 1 << 5; }
      if (state.data_fifo.size() !=  0) { result |= 1 << 6; }

      return result;
    }

    case 1: return read_resp_byte();
    case 2: return read_data_byte();
    case 3:
      switch (state.index & 1) {
        default: return state.interrupt_enable; // interrupt enable register
        case  1: return state.interrupt_request; // interrupt flag register
      }
  }

  return 0;
}

void cdrom::bus_write(int width, uint32_t address, uint32_t data) {
  assert(width == bus::BUS_WIDTH_BYTE);

  switch (address - 0x1f801800) {
    case 0:
      state.index = data & 3;
      return;

    case 1:
      switch (state.index) {
        case 0: // command register
          state.command = uint8_t(data);
          state.has_command = true;
          break;

        case 1: break; // sound map data out
        case 2: break; // sound map coding info
        case 3: break; // audio volume for cd-right to spu-right
      }
      break;

    case 2:
      switch (state.index) {
        case 0: // parameter fifo
          write_arg_byte(uint8_t(data));
          break;

        case 1: // interrupt enable register
          state.interrupt_enable = data;
          break;

        case 2: break; // audio volume for cd-right to spu-left
        case 3: break; // audio volume for cd-right to spu-left
      }
      break;

    case 3:
      switch (state.index) {
        case 0: break; // request register

        case 1: // interrupt flag register
          state.interrupt_request &= ~data;
          break;

        case 2: break; // audio volume for cd-left to spu-right
        case 3: break; // apply volume changes
      }
      break;
  }
}

static void (*second_response)() = nullptr;

static void command_get_stat() {
  write_resp_byte(0x02);

  cdrom::state.interrupt_request = 3;
  bus::irq(2);
}

static void command_test() {
  switch (read_arg_byte()) {
    case 0x20:
      write_resp_byte(0x99);
      write_resp_byte(0x02);
      write_resp_byte(0x01);
      write_resp_byte(0xc3);

      cdrom::state.interrupt_request = 3;
      bus::irq(2);
      break;
  }
}

static void command_get_id_no_disk() {
  write_resp_byte(0x08);
  write_resp_byte(0x40);

  write_resp_byte(0x00);
  write_resp_byte(0x00);

  write_resp_byte(0x00);
  write_resp_byte(0x00);
  write_resp_byte(0x00);
  write_resp_byte(0x00);

  cdrom::state.interrupt_request = 5;
  bus::irq(2);
}

static void command_get_id() {
  write_resp_byte(0x02);

  cdrom::state.interrupt_request = 3;
  bus::irq(2);

  second_response = &command_get_id_no_disk;
}

void cdrom::run() {
  if (second_response) {
    second_response();
    second_response = nullptr;
  }

  if (state.has_command) {
    state.has_command = false;

    switch (state.command) {
      case 0x01:
        return command_get_stat();

      case 0x19:
        return command_test();

      case 0x1a:
        return command_get_id();

      default:
        printf("cd-rom command: $%02x\n", state.command);
        break;
    }
  }
}
