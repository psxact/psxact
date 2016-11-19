#include <cassert>
#include "cdrom_core.hpp"
#include "../bus.hpp"

cdrom::state_t cdrom::state;

static uint8_t get_arg() {
  auto parameter = cdrom::state.args_fifo.front();
  cdrom::state.args_fifo.pop_front();

  return parameter;
}

static void set_arg(uint8_t data) {
  cdrom::state.args_fifo.push_back(data);
}

static uint8_t get_resp() {
  auto response = cdrom::state.resp_fifo.front();
  cdrom::state.resp_fifo.pop_front();

  return response;
}

static void set_resp(uint8_t data) {
  cdrom::state.resp_fifo.push_back(data);
}

static uint8_t get_data() {
  auto data = cdrom::state.data_fifo.front();
  cdrom::state.data_fifo.pop_front();

  return data;
}

static void set_data(uint8_t data) {
  cdrom::state.data_fifo.push_back(data);
}

uint32_t cdrom::mmio_read(int size, uint32_t address) {
  assert(size == BYTE);

  printf("cdrom::mmio_read(0x%08x)\n", address);

  switch (address - 0x1f801800) {
    case 0: { // status register
      uint32_t result = state.index;

      if (state.args_fifo.size() ==  0) { result |= 1 << 3; }
      if (state.args_fifo.size() != 16) { result |= 1 << 4; }
      if (state.resp_fifo.size() !=  0) { result |= 1 << 5; }
      if (state.data_fifo.size() !=  0) { result |= 1 << 6; }

      return result;
    }

    case 1: return get_resp();
    case 2: return get_data();
    case 3:
      switch (state.index & 1) {
        default: return state.interrupt_enable; // interrupt enable register
        case  1: return state.interrupt_request; // interrupt flag register
      }
  }

  return 0;
}

static void (*response2)();

static void get_id_response_no_disk() {
  set_resp(0x08); set_resp(0x40);
  set_resp(0x00); set_resp(0x00);
  // the bios isn't interested in these, and the current fifo implementation
  // could cause memory leaks.
  //
  // set_resp(0x00); set_resp(0x00); set_resp(0x00); set_resp(0x00);

  cdrom::state.interrupt_request = 5;
  bus::irq_req(2);

  response2 = nullptr;
}

static void command_get_stat() {
  set_resp(0);

  cdrom::state.interrupt_request = 3;
  bus::irq_req(2);
}

static void command_test() {
  switch (get_arg()) {
    case 0x20:
      set_resp(96);
      set_resp(1);
      set_resp(1);
      set_resp(2);

      cdrom::state.interrupt_request = 3;
      bus::irq_req(2);
      break;
  }
}

static void command_get_id() {
  set_resp(0);

  cdrom::state.interrupt_request = 3;
  bus::irq_req(2);

  response2 = &get_id_response_no_disk;
}

void cdrom::mmio_write(int size, uint32_t address, uint32_t data) {
  assert(size == BYTE);

  printf("cdrom::mmio_write(0x%08x, 0x%02x)\n", address, data);

  switch (address - 0x1f801800) {
    case 0:
      state.index = data & 3;
      return;

    case 1:
      switch (state.index) {
        case 0: // command register
          switch (data & 0xff) {
            case 0x01:
              return command_get_stat();

            case 0x19:
              return command_test();

            case 0x1a:
              return command_get_id();

            default:
              printf("cd-rom command: $%02x\n", data);
              break;
          }
          break;

        case 1: // sound map data out
          break;

        case 2: // sound map coding info
          break;

        case 3: // audio volume for cd-right to spu-right
          break;
      }
      break;

    case 2:
      switch (state.index) {
        case 0: // parameter fifo
          set_arg(uint8_t(data));
          break;

        case 1: // interrupt enable register
          state.interrupt_enable = data;
          break;

        case 2: // audio volume for cd-left to spu-left
          break;

        case 3: // audio volume for cd-right to spu-left
          break;
      }
      break;

    case 3:
      switch (state.index) {
        case 0: // request register
          break;

        case 1: // interrupt flag register
          state.interrupt_request &= ~data;

          if (response2) {
            response2();
          }
          break;

        case 2: // audio volume for cd-left to spu-right
          break;

        case 3: // apply volume changes
          break;
      }
      break;
  }
}
