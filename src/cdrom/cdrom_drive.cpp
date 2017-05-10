#include <cassert>
#include "cdrom_drive.hpp"
#include "../bus.hpp"

static uint8_t read_args_byte(cdrom_state_t *state) {
  return state->args_fifo.read();
}

static void write_args_byte(cdrom_state_t *state, uint8_t data) {
  state->args_fifo.write(data);
}

static uint8_t read_resp_byte(cdrom_state_t *state) {
  return state->resp_fifo.read();
}

static void write_resp_byte(cdrom_state_t *state, uint8_t data) {
  state->resp_fifo.write(data);
}

static uint8_t read_data_byte(cdrom_state_t *state) {
  return state->data_fifo.read();
}

static void write_data_byte(cdrom_state_t *state, uint8_t data) {
  state->data_fifo.write(data);
}

uint32_t cdrom::io_read(cdrom_state_t *state, int width, uint32_t address) {
  assert(width == bus::BUS_WIDTH_BYTE);

  switch (address - 0x1f801800) {
  case 0: // status register
    return
      state->index |
      ( state->args_fifo.is_empty() << 3) |
      (!state->args_fifo.is_full () << 4) |
      (!state->resp_fifo.is_empty() << 5) |
      (!state->data_fifo.is_empty() << 6);

  case 1: return read_resp_byte(state);
  case 2: return read_data_byte(state);
  case 3:
    switch (state->index & 1) {
    case 0: return state->interrupt_enable; // interrupt enable register
    case 1: return state->interrupt_request; // interrupt flag register
    }
  }

  return 0;
}

void cdrom::io_write(cdrom_state_t *state, int width, uint32_t address, uint32_t data) {
  assert(width == bus::BUS_WIDTH_BYTE);

  switch (address - 0x1f801800) {
  case 0:
    state->index = data & 3;
    return;

  case 1:
    switch (state->index) {
    case 0: // command register
      state->command = uint8_t(data);
      state->has_command = true;
      break;

    case 1: break; // sound map data out
    case 2: break; // sound map coding info
    case 3: break; // audio volume for cd-right to spu-right
    }
    break;

  case 2:
    switch (state->index) {
    case 0: // parameter fifo
      write_args_byte(state, uint8_t(data));
      break;

    case 1: // interrupt enable register
      state->interrupt_enable = data;
      break;

    case 2: break; // audio volume for cd-right to spu-left
    case 3: break; // audio volume for cd-right to spu-left
    }
    break;

  case 3:
    switch (state->index) {
    case 0: break; // request register

    case 1: // interrupt flag register
      state->interrupt_request &= ~data;
      break;

    case 2: break; // audio volume for cd-left to spu-right
    case 3: break; // apply volume changes
    }
    break;
  }
}

typedef void (*action_t)(cdrom_state_t *state);

static action_t second_response = nullptr;

static void command_get_stat(cdrom_state_t *state) {
  write_resp_byte(state, 0x02);

  state->interrupt_request = 3;
  bus::irq(2);
}

static void command_test(cdrom_state_t *state) {
  switch (read_args_byte(state)) {
  case 0x20:
    write_resp_byte(state, 0x99);
    write_resp_byte(state, 0x02);
    write_resp_byte(state, 0x01);
    write_resp_byte(state, 0xc3);

    state->interrupt_request = 3;
    bus::irq(2);
    break;
  }
}

static void command_get_id_no_disk(cdrom_state_t *state) {
  write_resp_byte(state, 0x08);
  write_resp_byte(state, 0x40);

  write_resp_byte(state, 0x00);
  write_resp_byte(state, 0x00);

  write_resp_byte(state, 0x00);
  write_resp_byte(state, 0x00);
  write_resp_byte(state, 0x00);
  write_resp_byte(state, 0x00);

  state->interrupt_request = 5;
  bus::irq(2);
}

static void command_get_id(cdrom_state_t *state) {
  write_resp_byte(state, 0x02);

  state->interrupt_request = 3;
  bus::irq(2);

  second_response = &command_get_id_no_disk;
}

void cdrom::run(cdrom_state_t *state) {
  if (second_response) {
    second_response(state);
    second_response = nullptr;
  }

  if (state->has_command) {
    state->has_command = false;

    switch (state->command) {
    case 0x01:
      return command_get_stat(state);

    case 0x19:
      return command_test(state);

    case 0x1a:
      return command_get_id(state);

    default:
      printf("cd-rom command: $%02x\n", state->command);
      break;
    }
  }
}
