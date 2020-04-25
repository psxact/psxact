#include "cdrom/core.hpp"

#include "util/bcd.hpp"

using namespace psx::cdrom;
using namespace psx::util;

core_t::core_t(interrupt_access_t *irq, const char *game_file_name, bool log_enabled)
    : addressable_t("cdc", log_enabled)
    , irq(irq)
    , index()
    , interrupt_enable()
    , interrupt_request()
    , interrupt_timer()
    , seek_timecode()
    , read_timecode()
    , seek_unprocessed()
    , parameter_fifo()
    , response_fifo()
    , rx_buffer() // TODO: Replace with heap-allocation
    , rx_index()
    , rx_active()
    , command()
    , command_unprocessed()
    , busy()
    , is_seeking()
    , is_reading()
    , game_file_name(game_file_name)
    , game_file()
    , logic()
    , drive()
    , mode() {
  game_file = fopen(game_file_name, "rb+");

  logic_transition(&core_t::logic_idling, 1000);
  drive_transition(&core_t::drive_idling, 1000);
}

void core_t::tick(int amount) {
  while (amount) {
    amount--;

    drive.timer--;

    if (drive.timer == 0) {
      (*this.*drive.stage)();
    }

    logic.timer--;

    if (logic.timer == 0) {
      (*this.*logic.stage)();
    }

    if (interrupt_timer) {
      interrupt_timer--;

      if (interrupt_timer == 0 && interrupt_request) {
        int32_t signal = interrupt_request & interrupt_enable;
        if (signal == interrupt_request) {
          log("delivering interrupt: %d", interrupt_request);
          irq->send(interrupt_type_t::CDROM);
        }
      }
    }
  }
}

uint8_t core_t::get_status_byte() {
  uint8_t result = 0x02;

  if (is_seeking) {
    result |= 0x40;
  }

  if (is_reading) {
    result |= 0x20;
  }

  return result;
}

int core_t::get_cycles_per_sector() {
  if (mode.double_speed) {
    return (33868800 * 2) / 150;
  }
  else {
    return (33868800 * 2) / 75;
  }
}

int32_t core_t::get_read_cursor() {
  constexpr int sectors_per_second = 75;
  constexpr int seconds_per_minute = 60;
  constexpr int sectors_per_minute = seconds_per_minute * sectors_per_second;
  constexpr int bytes_per_sector = 2352;
  constexpr int lead_in_duration = 2 * sectors_per_second;

  int cursor =
    (read_timecode.minute * sectors_per_minute) +
    (read_timecode.second * sectors_per_second) +
    (read_timecode.sector);

  return bytes_per_sector * (cursor - lead_in_duration);
}

void core_t::read_sector() {
  log("read_sector(\"%02d:%02d:%02d\")",
    read_timecode.minute,
    read_timecode.second,
    read_timecode.sector);

  if (mode.read_whole_sector) {
    rx_index = 12;
    rx_len = 0x930;
  }
  else {
    rx_index = 24;
    rx_len = 0x818;
  }

  is_reading = 1;

  int32_t cursor = get_read_cursor();

  fseek(game_file, cursor, SEEK_SET);
  fread(rx_buffer, sizeof(uint8_t), 0x930, game_file);

  auto minute = bcd::to_dec(rx_buffer[12]);
  auto second = bcd::to_dec(rx_buffer[13]);
  auto sector = bcd::to_dec(rx_buffer[14]);

  if (
    minute != read_timecode.minute ||
    second != read_timecode.second ||
    sector != read_timecode.sector) {
    log("expecting \"%02d:%02d:%02d\", but got \"%02d:%02d:%02d\" at 0x%08x",
      read_timecode.minute,
      read_timecode.second,
      read_timecode.sector,
      minute,
      second,
      sector,
      cursor);
  }
}

// -========-
//  Commands
// -========-

void core_t::do_seek() {
  if (seek_unprocessed) {
    seek_unprocessed = 0;
    read_timecode.minute = seek_timecode.minute;
    read_timecode.second = seek_timecode.second;
    read_timecode.sector = seek_timecode.sector;
  }
}

void core_t::command_get_id() {
  logic.response_fifo.write(get_status_byte());
  logic.interrupt_request = 3;

  drive_transition(&core_t::drive_getting_id, 40000);
}

void core_t::command_get_status() {
  logic.response_fifo.write(get_status_byte());
  logic.interrupt_request = 3;
}

void core_t::command_init() {
  logic.response_fifo.write(get_status_byte());
  logic.interrupt_request = 3;

  drive_transition(&core_t::drive_int2, 1000);
}

void core_t::command_pause() {
  logic.response_fifo.write(get_status_byte());
  logic.interrupt_request = 3;

  is_reading = 0;

  drive_transition(&core_t::drive_int2, 1);
}

void core_t::command_read_n() {
  logic.response_fifo.write(get_status_byte());
  logic.interrupt_request = 3;

  do_seek();

  int cycles = get_cycles_per_sector();

  drive_transition(&core_t::drive_reading, cycles);
}

void core_t::command_read_table_of_contents() {
  logic.response_fifo.write(get_status_byte());
  logic.interrupt_request = 3;

  drive_transition(&core_t::drive_int2, 40000);
}

void core_t::command_seek_data_mode() {
  logic.response_fifo.write(get_status_byte());
  logic.interrupt_request = 3;

  do_seek();

  drive_transition(&core_t::drive_int2, 40000);
}

void core_t::command_set_mode(uint8_t value) {
  logic.response_fifo.write(get_status_byte());
  logic.interrupt_request = 3;

  mode.double_speed = (value & 0x80) != 0;
  mode.read_whole_sector = (value & 0x20) != 0;
}

void core_t::command_set_seek_target(uint8_t minute, uint8_t second, uint8_t sector) {
  logic.response_fifo.write(get_status_byte());
  logic.interrupt_request = 3;

  seek_timecode.minute = minute;
  seek_timecode.second = second;
  seek_timecode.sector = sector;
  seek_unprocessed = 1;
}

void core_t::command_test(uint8_t function) {
  log("command_test(0x%02x)", function);

  switch (function) {
  case 0x20:
    logic.response_fifo.write(0x98);
    logic.response_fifo.write(0x06);
    logic.response_fifo.write(0x10);
    logic.response_fifo.write(0xc3);
    logic.interrupt_request = 3;
    break;
  }
}

void core_t::command_unmute() {
  logic.response_fifo.write(get_status_byte());
  logic.interrupt_request = 3;
}

// -=====-
//  Logic
// -=====-

void core_t::logic_transition(stage_t stage, int timer) {
  logic.stage = stage;
  logic.timer = timer;
}

void core_t::logic_idling() {
  if (command_unprocessed) {
    command_unprocessed = 0;

    if (parameter_fifo.is_empty()) {
      logic_transition(&core_t::logic_transferring_command, 1000);
    }
    else {
      logic_transition(&core_t::logic_transferring_parameters, 1000);
    }
  }
  else {
    logic_transition(&core_t::logic_idling, 1000);
  }
}

void core_t::logic_transferring_parameters() {
  logic.parameter_fifo.write(parameter_fifo.read());

  if (parameter_fifo.is_empty()) {
    logic_transition(&core_t::logic_transferring_command, 1000);
  }
  else {
    logic_transition(&core_t::logic_transferring_parameters, 1000);
  }
}

void core_t::logic_transferring_command() {
  logic.command = command;

  logic_transition(&core_t::logic_executing_command, 1000);
}

void core_t::logic_executing_command() {
#define get_param() \
  logic.parameter_fifo.read()

  log("logic_executing_command(0x%02x)", logic.command);

  switch (logic.command) {
  case 0x01:
    command_get_status();
    break;

  case 0x02: {
    uint8_t minute = bcd::to_dec(get_param());
    uint8_t second = bcd::to_dec(get_param());
    uint8_t sector = bcd::to_dec(get_param());

    command_set_seek_target(minute, second, sector);
    break;
  }

  case 0x06:
    command_read_n();
    break;

  case 0x09:
    command_pause();
    break;

  case 0x0a:
    command_init();
    break;

  case 0x0c:
    command_unmute();
    break;

  case 0x0e: {
    uint8_t mode = get_param();

    command_set_mode(mode);
    break;
  }

  case 0x15:
    command_seek_data_mode();
    break;

  case 0x19: {
    uint8_t function = get_param();

    command_test(function);
    break;
  }

  case 0x1a:
    command_get_id();
    break;

  case 0x1e:
    command_read_table_of_contents();
    break;

  default:
    log("unknown command `0x%02x'", command);
    return;
  }

  logic_transition(&core_t::logic_clearing_response, 1000);

#undef get_param
}

void core_t::logic_clearing_response() {
  response_fifo.clear();

  logic_transition(&core_t::logic_transferring_response, 1000);
}

void core_t::logic_transferring_response() {
  response_fifo.write(logic.response_fifo.read());

  if (logic.response_fifo.is_empty()) {
    logic_transition(&core_t::logic_deliver_interrupt, 1000);
  }
  else {
    logic_transition(&core_t::logic_transferring_response, 1000);
  }
}

void core_t::logic_deliver_interrupt() {
  if (interrupt_request == 0) {
    interrupt_timer = 500;
    interrupt_request = logic.interrupt_request;

    logic_transition(&core_t::logic_idling, 1);
  }
  else {
    logic_transition(&core_t::logic_deliver_interrupt, 1);
  }
}

// -=====-
//  Drive
// -=====-

void core_t::drive_transition(stage_t stage, int timer) {
  drive.stage = stage;
  drive.timer = timer;
}

void core_t::drive_idling() {
}

void core_t::drive_getting_id() {
  if (interrupt_request == 0) {
    // INT2(02h,00h, 20h,00h, 53h,43h,45h,4xh)

    logic.response_fifo.write(0x02);
    logic.response_fifo.write(0x00);

    logic.response_fifo.write(0x20);
    logic.response_fifo.write(0x00);

    logic.response_fifo.write('S');
    logic.response_fifo.write('C');
    logic.response_fifo.write('E');
    logic.response_fifo.write('A');
    logic.interrupt_request = 2;

    drive_transition(&core_t::drive_idling, 1000);
    logic_transition(&core_t::logic_clearing_response, 1000);
  }
  else {
    drive_transition(&core_t::drive_getting_id, 1000);
  }
}

void core_t::drive_int2() {
  if (interrupt_request == 0) {
    logic.response_fifo.write(get_status_byte());
    logic.interrupt_request = 2;

    drive_transition(&core_t::drive_idling, 1000);
    logic_transition(&core_t::logic_clearing_response, 1000);
  }
  else {
    drive_transition(&core_t::drive_int2, 1000);
  }
}

void core_t::drive_reading() {
  logic.response_fifo.write(get_status_byte());
  logic.interrupt_request = 1;

  read_sector();

  read_timecode.sector++;

  if (read_timecode.sector == 75) {
    read_timecode.sector = 0;
    read_timecode.second++;

    if (read_timecode.second == 60) {
      read_timecode.second = 0;
      read_timecode.minute++;
    }
  }

  // continually read

  int cycles = get_cycles_per_sector();

  drive_transition(&core_t::drive_reading, cycles);
  logic_transition(&core_t::logic_clearing_response, 1000);
}
