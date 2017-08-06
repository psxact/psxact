#include <cassert>
#include "cdrom_drive.hpp"
#include "../utility.hpp"

void cdrom::init(cdrom_state_t &state, const char *game_file_name) {
  state.game_file_name = game_file_name;
  state.game_file = fopen(game_file_name, "rb+");

  cdrom::logic::transition(state, &cdrom::logic::idling, 1000);
  cdrom::drive::transition(state, &cdrom::drive::idling, 1000);
}

static uint32_t get_port(uint32_t address) {
  return address - 0x1f801800;
}

static uint8_t io_read_port_0(cdrom_state_t &state) {
  return uint8_t(
    (state.index) |
  //(state.xa_adpcm.has_data() << 2) |
    (state.parameter.is_empty() << 3) |
    (state.parameter.has_room() << 4) |
    (state.response.has_data() << 5) |
    (state.data.has_data() << 6) |
    (state.busy << 7)
  );
}

static uint8_t io_read_port_1(cdrom_state_t &state) {
  return state.response.read();
}

static uint8_t io_read_port_2(cdrom_state_t &state) {
  return state.data.read();
}

static uint8_t io_read_port_3(cdrom_state_t &state) {
  switch (state.index) {
  case 0:
  case 2:
    return uint8_t(0xe0 | state.interrupt_enable);

  case 1:
  case 3:
    return uint8_t(0xe0 | state.interrupt_request);

  default:
    return 0;
  }
}

static uint32_t io_read_internal(cdrom_state_t &state, uint32_t port) {
  switch (port) {
  case 0:
    return io_read_port_0(state);

  case 1:
    return io_read_port_1(state);

  case 2:
    return io_read_port_2(state);

  case 3:
    return io_read_port_3(state);

  default:
    return 0;
  }
}

uint32_t cdrom::io_read(cdrom_state_t &state, bus::bus_width_t width, uint32_t address) {
  if (width == bus::BUS_WIDTH_WORD && address == 0x1f801800) {
    uint8_t b0 = state.data.read();
    uint8_t b1 = state.data.read();
    uint8_t b2 = state.data.read();
    uint8_t b3 = state.data.read();

    return (b0 << 0) | (b1 << 8) | (b2 << 16) | (b3 << 24);
  }

  assert(width == bus::BUS_WIDTH_BYTE);

  uint32_t port = get_port(address);
  uint32_t data = io_read_internal(state, port);

  if (utility::log_cdrom) {
    printf("cdrom::io_read_port_%d_%d() returned 0x%02x\n", port, state.index, data);
  }

  return data;
}

static void io_write_port_0_n(cdrom_state_t &state, uint8_t data) {
  state.index = data & 3;
}

static void io_write_port_1_0(cdrom_state_t &state, uint8_t data) {
  state.command = data;
  state.command_is_new = 1;
}

static void io_write_port_1_1(cdrom_state_t &state, uint8_t data) {
}

static void io_write_port_1_2(cdrom_state_t &state, uint8_t data) {
}

static void io_write_port_1_3(cdrom_state_t &state, uint8_t data) {
}

static void io_write_port_2_0(cdrom_state_t &state, uint8_t data) {
  state.parameter.write(data);
}

static void io_write_port_2_1(cdrom_state_t &state, uint8_t data) {
  int32_t flags = data & 0x1f;
  state.interrupt_enable = flags;
}

static void io_write_port_2_2(cdrom_state_t &state, uint8_t data) {
}

static void io_write_port_2_3(cdrom_state_t &state, uint8_t data) {
}

static void io_write_port_3_0(cdrom_state_t &state, uint8_t data) {
  if (data & 0x80) {
    int skip = state.mode.read_whole_sector ? 12 : 24;
    int size = state.mode.read_whole_sector ? 0x924 : 0x800;

    state.data.clear();

    for (int i = 0; i < size; i++) {
      state.data.write(state.data_buffer[i + skip]);
    }
  }
  else {
    state.data.clear();
  }
}

static void io_write_port_3_1(cdrom_state_t &state, uint8_t data) {
  int32_t flags = data & 0x1f;
  state.interrupt_request &= ~flags;

  if (data & 0x40) {
    state.parameter.clear();
  }
}

static void io_write_port_3_2(cdrom_state_t &state, uint8_t data) {
}

static void io_write_port_3_3(cdrom_state_t &state, uint8_t data) {
}

void cdrom::io_write(cdrom_state_t &state, bus::bus_width_t width, uint32_t address, uint32_t data) {
  assert(width == bus::BUS_WIDTH_BYTE);

  uint32_t port = get_port(address);

  if (utility::log_cdrom) {
    printf("cdrom::io_write_port_%d_%d(0x%02x)\n", port, state.index, data);
  }

  uint8_t clipped = uint8_t(data);

  switch (port) {
  case 0:
    return io_write_port_0_n(state, clipped);

  case 1:
    switch (state.index) {
    case 0: return io_write_port_1_0(state, clipped);
    case 1: return io_write_port_1_1(state, clipped);
    case 2: return io_write_port_1_2(state, clipped);
    case 3: return io_write_port_1_3(state, clipped);
    }

  case 2:
    switch (state.index) {
    case 0: return io_write_port_2_0(state, clipped);
    case 1: return io_write_port_2_1(state, clipped);
    case 2: return io_write_port_2_2(state, clipped);
    case 3: return io_write_port_2_3(state, clipped);
    }

  case 3:
    switch (state.index) {
    case 0: return io_write_port_3_0(state, clipped);
    case 1: return io_write_port_3_1(state, clipped);
    case 2: return io_write_port_3_2(state, clipped);
    case 3: return io_write_port_3_3(state, clipped);
    }
  }
}

void cdrom::tick(cdrom_state_t &state) {
  state.drive.timer--;

  if (state.drive.timer == 0) {
    state.drive.stage(state);
  }

  state.logic.timer--;

  if (state.logic.timer == 0) {
    state.logic.stage(state);
  }

  if (state.interrupt_request) {
    int32_t signal = state.interrupt_request & state.interrupt_enable;
    if (signal == state.interrupt_request) {
      bus::irq(2);
    }
  }
}

uint8_t cdrom::get_status_byte(cdrom_state_t &state) {
  return 0x02;
}

static int get_cycles_per_sector(cdrom_state_t &state) {
  if (state.mode.double_speed) {
    return 33868800 / 150;
  }
  else {
    return 33868800 / 75;
  }
}

void cdrom::read_sector(cdrom_state_t &state) {
  constexpr int sectors_per_second = 75;
  constexpr int sectors_per_minute = 60 * sectors_per_second;
  constexpr int bytes_per_sector = 2352;
  constexpr int lead_in_duration = 2 * sectors_per_second;

  if (utility::log_cdrom) {
    printf("cdrom::read_sector(\"%02d:%02d:%02d\")\n",
           state.read_timecode.minute,
           state.read_timecode.second,
           state.read_timecode.sector);
  }

  cdrom_sector_timecode_t &tc = state.read_timecode;
  int32_t cursor =
      (tc.minute * sectors_per_minute) +
      (tc.second * sectors_per_second) +
      (tc.sector);

  int32_t target = bytes_per_sector * (cursor - lead_in_duration);

  fseek(state.game_file, target, SEEK_SET);
  fread(state.data_buffer, sizeof(uint8_t), 0x930, state.game_file);
}

// -========-
//  Commands
// -========-

static void do_seek(cdrom_state_t &state) {
  if (state.seek_unprocessed) {
    state.seek_unprocessed = 0;
    state.read_timecode.minute = state.seek_timecode.minute;
    state.read_timecode.second = state.seek_timecode.second;
    state.read_timecode.sector = state.seek_timecode.sector;
  }
}

void cdrom::command::get_id(cdrom_state_t &state) {
  state.logic.response.write(cdrom::get_status_byte(state));
  state.logic.interrupt_request = 3;

  cdrom::drive::transition(state, &cdrom::drive::getting_id, 40000);
}

void cdrom::command::get_status(cdrom_state_t &state) {
  state.logic.response.write(cdrom::get_status_byte(state));
  state.logic.interrupt_request = 3;
}

void cdrom::command::init(cdrom_state_t &state) {
  state.logic.response.write(cdrom::get_status_byte(state));
  state.logic.interrupt_request = 3;

  cdrom::drive::transition(state, &cdrom::drive::int2, 1000);
}

void cdrom::command::pause(cdrom_state_t &state) {
  state.logic.response.write(cdrom::get_status_byte(state));
  state.logic.interrupt_request = 3;

  cdrom::drive::transition(state, &cdrom::drive::int2, 1000);
}

void cdrom::command::read_n(cdrom_state_t &state) {
  state.logic.response.write(cdrom::get_status_byte(state));
  state.logic.interrupt_request = 3;

  do_seek(state);

  int cycles = get_cycles_per_sector(state);

  cdrom::drive::transition(state, &cdrom::drive::reading, cycles);
}

void cdrom::command::read_table_of_contents(cdrom_state_t &state) {
  state.logic.response.write(cdrom::get_status_byte(state));
  state.logic.interrupt_request = 3;

  cdrom::drive::transition(state, &cdrom::drive::int2, 40000);
}

void cdrom::command::seek_data_mode(cdrom_state_t &state) {
  state.logic.response.write(cdrom::get_status_byte(state));
  state.logic.interrupt_request = 3;

  do_seek(state);

  cdrom::drive::transition(state, &cdrom::drive::int2, 40000);
}

void cdrom::command::set_mode(cdrom_state_t &state, uint8_t mode) {
  state.logic.response.write(cdrom::get_status_byte(state));
  state.logic.interrupt_request = 3;

  state.mode.double_speed = (mode & 0x80) != 0;
  state.mode.read_whole_sector = (mode & 0x20) != 0;
}

void cdrom::command::set_seek_target(cdrom_state_t &state, uint8_t minute, uint8_t second, uint8_t sector) {
  state.logic.response.write(cdrom::get_status_byte(state));
  state.logic.interrupt_request = 3;

  state.seek_timecode.minute = minute;
  state.seek_timecode.second = second;
  state.seek_timecode.sector = sector;
  state.seek_unprocessed = 1;
}

void cdrom::command::test(cdrom_state_t &state, uint8_t function) {
  if (utility::log_cdrom) {
    printf("cdrom::command::test(0x%02x)\n", function);
  }

  switch (function) {
  case 0x20:
    state.logic.response.write(0x98);
    state.logic.response.write(0x06);
    state.logic.response.write(0x10);
    state.logic.response.write(0xc3);
    state.logic.interrupt_request = 3;
    break;
  }
}

void cdrom::command::unmute(cdrom_state_t &state) {
  state.logic.response.write(cdrom::get_status_byte(state));
  state.logic.interrupt_request = 3;
}

// -=====-
//  Logic
// -=====-

void cdrom::logic::transition(cdrom_state_t &state, cdrom_state_t::stage_t stage, int timer) {
  state.logic.stage = stage;
  state.logic.timer = timer;
}

void cdrom::logic::idling(cdrom_state_t &state) {
  if (state.command_is_new) {
    state.command_is_new = 0;

    if (state.parameter.is_empty()) {
      transition(state, cdrom::logic::transferring_command, 1000);
    }
    else {
      transition(state, cdrom::logic::transferring_parameters, 1000);
    }
  }
  else {
    transition(state, cdrom::logic::idling, 1000);
  }
}

void cdrom::logic::transferring_parameters(cdrom_state_t &state) {
  state.logic.parameter.write(state.parameter.read());

  if (state.parameter.is_empty()) {
    transition(state, cdrom::logic::transferring_command, 1000);
  }
  else {
    transition(state, cdrom::logic::transferring_parameters, 1000);
  }
}

void cdrom::logic::transferring_command(cdrom_state_t &state) {
  state.logic.command = state.command;

  transition(state, cdrom::logic::executing_command, 1000);
}

void cdrom::logic::executing_command(cdrom_state_t &state) {
#define get_param() state.logic.parameter.read()

  if (utility::log_cdrom) {
    printf("cdrom::control::executing_command(0x%02x)\n", state.logic.command);
  }

  switch (state.logic.command) {
  case 0x01:
    cdrom::command::get_status(state);
    break;

  case 0x02: {
    uint8_t minute = utility::bcd_to_dec(get_param());
    uint8_t second = utility::bcd_to_dec(get_param());
    uint8_t sector = utility::bcd_to_dec(get_param());

    cdrom::command::set_seek_target(state, minute, second, sector);
    break;
  }

  case 0x06:
    cdrom::command::read_n(state);
    break;

  case 0x09:
    cdrom::command::pause(state);
    break;

  case 0x0a:
    cdrom::command::init(state);
    break;

  case 0x0c:
    cdrom::command::unmute(state);
    break;

  case 0x0e: {
    uint8_t mode = get_param();

    cdrom::command::set_mode(state, mode);
    break;
  }

  case 0x15:
    cdrom::command::seek_data_mode(state);
    break;

  case 0x19: {
    uint8_t function = get_param();

    cdrom::command::test(state, function);
    break;
  }

  case 0x1a:
    cdrom::command::get_id(state);
    break;

  case 0x1e:
    cdrom::command::read_table_of_contents(state);
    break;

  default:
    return;
  }

  transition(state, &cdrom::logic::clearing_response, 1000);

#undef get_param
}

void cdrom::logic::clearing_response(cdrom_state_t &state) {
  state.response.clear();

  transition(state, &cdrom::logic::transferring_response, 1000);
}

void cdrom::logic::transferring_response(cdrom_state_t &state) {
  state.response.write(state.logic.response.read());

  if (state.logic.response.is_empty()) {
    transition(state, &cdrom::logic::deliver_interrupt, 1000);
  }
  else {
    transition(state, &cdrom::logic::transferring_response, 1000);
  }
}

void cdrom::logic::deliver_interrupt(cdrom_state_t &state) {
  if (state.interrupt_request == 0) {
    state.interrupt_request = state.logic.interrupt_request;

    transition(state, &cdrom::logic::idling, 1);
  }
  else {
    transition(state, &cdrom::logic::deliver_interrupt, 1);
  }
}

// -=====-
//  Drive
// -=====-

void cdrom::drive::transition(cdrom_state_t &state, cdrom_state_t::stage_t stage, int timer) {
  state.drive.stage = stage;
  state.drive.timer = timer;
}

void cdrom::drive::idling(cdrom_state_t &state) {

}

void cdrom::drive::getting_id(cdrom_state_t &state) {
  if (state.interrupt_request == 0) {
    // INT2(02h,00h, 20h,00h, 53h,43h,45h,4xh)

    state.logic.response.write(0x02);
    state.logic.response.write(0x00);

    state.logic.response.write(0x20);
    state.logic.response.write(0x00);

    state.logic.response.write('S');
    state.logic.response.write('C');
    state.logic.response.write('E');
    state.logic.response.write('A');
    state.logic.interrupt_request = 2;

    cdrom::drive::transition(state, cdrom::drive::idling, 1000);
    cdrom::logic::transition(state, cdrom::logic::clearing_response, 1000);
  }
  else {
    cdrom::drive::transition(state, cdrom::drive::getting_id, 1000);
  }
}

void cdrom::drive::int2(cdrom_state_t &state) {
  if (state.interrupt_request == 0) {
    state.logic.response.write(cdrom::get_status_byte(state));
    state.logic.interrupt_request = 2;

    cdrom::drive::transition(state, cdrom::drive::idling, 1000);
    cdrom::logic::transition(state, cdrom::logic::clearing_response, 1000);
  }
  else {
    cdrom::drive::transition(state, cdrom::drive::int2, 1000);
  }
}

void cdrom::drive::reading(cdrom_state_t &state) {
  state.logic.response.write(cdrom::get_status_byte(state));
  state.logic.interrupt_request = 1;

  cdrom::read_sector(state);

  state.read_timecode.sector++;

  if (state.read_timecode.sector == 75) {
    state.read_timecode.sector = 0;
    state.read_timecode.second++;

    if (state.read_timecode.second == 60) {
      state.read_timecode.second = 0;
      state.read_timecode.minute++;
    }
  }

  // continually read

  int cycles = get_cycles_per_sector(state);

  cdrom::drive::transition(state, &cdrom::drive::reading, cycles);
  cdrom::logic::transition(state, &cdrom::logic::clearing_response, 1000);
}
