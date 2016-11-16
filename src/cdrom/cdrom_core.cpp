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
      uint32_t result = 0x18;

      result |= state.index;

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

static void command_test() {
  switch (get_arg()) {
    case 0x20:
      set_resp(96);
      set_resp(01);
      set_resp(01);
      set_resp(02);
      break;
  }
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
            case 0x19: return command_test();

            default:
              printf("cd-rom command: $%02x\n", data);
              break;
          }
          break;

        case 1: break; // sound map data out
        case 2: break; // sound map coding info
        case 3: break; // audio volume for cd-right to spu-right
      }
      break;

    case 2:
      switch (state.index) {
        case 0: set_arg(uint8_t(data)); break; // parameter fifo
        case 1: state.interrupt_enable = data; break; // interrupt enable register
        case 2: break; // audio volume for cd-left to spu-left
        case 3: break; // audio volume for cd-right to spu-left
      }
      break;

    case 3:
      switch (state.index) {
        case 0: break; // request register
        case 1: state.interrupt_request &= ~data; break; // interrupt flag register
        case 2: break; // audio volume for cd-left to spu-right
        case 3: break; // apply volume changes
      }
      break;
  }
}
