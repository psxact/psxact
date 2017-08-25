#include "cdrom_core.hpp"
#include "../utility.hpp"

using namespace psxact::cdrom;

core::core(const char *game_file_name)
    : game_file_name(game_file_name) {
  game_file = fopen(game_file_name, "rb+");

  logic_transition(&core::logic_idling, 1000);
  drive_transition(&core::drive_idling, 1000);
}

void core::tick() {
  drive.timer--;

  if (drive.timer == 0) {
    (*this.*drive.stage)();
  }

  logic.timer--;

  if (logic.timer == 0) {
    (*this.*logic.stage)();
  }

  if (interrupt_request) {
    int32_t signal = interrupt_request & interrupt_enable;
    if (signal == interrupt_request) {
      system->irq(2);
    }
  }
}

uint8_t core::get_status_byte() {
  return 0x02;
}

int core::get_cycles_per_sector() {
  if (mode.double_speed) {
    return 33868800 / 150;
  } else {
    return 33868800 / 75;
  }
}

void core::read_sector() {
  constexpr int sectors_per_second = 75;
  constexpr int sectors_per_minute = 60 * sectors_per_second;
  constexpr int bytes_per_sector = 2352;
  constexpr int lead_in_duration = 2 * sectors_per_second;

  if (utility::log_cdrom) {
    printf("cdrom_core::read_sector(\"%02d:%02d:%02d\")\n",
           read_timecode.minute,
           read_timecode.second,
           read_timecode.sector);
  }

  sector_timecode_t &tc = read_timecode;
  int32_t cursor =
      (tc.minute * sectors_per_minute) +
      (tc.second * sectors_per_second) +
      (tc.sector);

  int32_t target = bytes_per_sector * (cursor - lead_in_duration);

  fseek(game_file, target, SEEK_SET);
  fread(data_buffer, sizeof(uint8_t), 0x930, game_file);
}

// -========-
//  Commands
// -========-

void core::do_seek() {
  if (seek_unprocessed) {
    seek_unprocessed = 0;
    read_timecode.minute = seek_timecode.minute;
    read_timecode.second = seek_timecode.second;
    read_timecode.sector = seek_timecode.sector;
  }
}

void core::command_get_id() {
  logic.response.write(get_status_byte());
  logic.interrupt_request = 3;

  drive_transition(&core::drive_getting_id, 40000);
}

void core::command_get_status() {
  logic.response.write(get_status_byte());
  logic.interrupt_request = 3;
}

void core::command_init() {
  logic.response.write(get_status_byte());
  logic.interrupt_request = 3;

  drive_transition(&core::drive_int2, 1000);
}

void core::command_pause() {
  logic.response.write(get_status_byte());
  logic.interrupt_request = 3;

  drive_transition(&core::drive_int2, 1000);
}

void core::command_read_n() {
  logic.response.write(get_status_byte());
  logic.interrupt_request = 3;

  do_seek();

  int cycles = get_cycles_per_sector();

  drive_transition(&core::drive_reading, cycles);
}

void core::command_read_table_of_contents() {
  logic.response.write(get_status_byte());
  logic.interrupt_request = 3;

  drive_transition(&core::drive_int2, 40000);
}

void core::command_seek_data_mode() {
  logic.response.write(get_status_byte());
  logic.interrupt_request = 3;

  do_seek();

  drive_transition(&core::drive_int2, 40000);
}

void core::command_set_mode(uint8_t value) {
  logic.response.write(get_status_byte());
  logic.interrupt_request = 3;

  mode.double_speed = (value & 0x80) != 0;
  mode.read_whole_sector = (value & 0x20) != 0;
}

void core::command_set_seek_target(uint8_t minute, uint8_t second, uint8_t sector) {
  logic.response.write(get_status_byte());
  logic.interrupt_request = 3;

  seek_timecode.minute = minute;
  seek_timecode.second = second;
  seek_timecode.sector = sector;
  seek_unprocessed = 1;
}

void core::command_test(uint8_t function) {
  if (utility::log_cdrom) {
    printf("cdrom_core::command_test(0x%02x)\n", function);
  }

  switch (function) {
  case 0x20:
    logic.response.write(0x98);
    logic.response.write(0x06);
    logic.response.write(0x10);
    logic.response.write(0xc3);
    logic.interrupt_request = 3;
    break;
  }
}

void core::command_unmute() {
  logic.response.write(get_status_byte());
  logic.interrupt_request = 3;
}

// -=====-
//  Logic
// -=====-

void core::logic_transition(stage_t stage, int timer) {
  logic.stage = stage;
  logic.timer = timer;
}

void core::logic_idling() {
  if (command_is_new) {
    command_is_new = 0;

    if (parameter.is_empty()) {
      logic_transition(&core::logic_transferring_command, 1000);
    } else {
      logic_transition(&core::logic_transferring_parameters, 1000);
    }
  } else {
    logic_transition(&core::logic_idling, 1000);
  }
}

void core::logic_transferring_parameters() {
  logic.parameter.write(parameter.read());

  if (parameter.is_empty()) {
    logic_transition(&core::logic_transferring_command, 1000);
  } else {
    logic_transition(&core::logic_transferring_parameters, 1000);
  }
}

void core::logic_transferring_command() {
  logic.command = command;

  logic_transition(&core::logic_executing_command, 1000);
}

void core::logic_executing_command() {
#define get_param() logic.parameter.read()

  if (utility::log_cdrom) {
    printf("cdrom_core::control::executing_command(0x%02x)\n", logic.command);
  }

  switch (logic.command) {
  case 0x01:
    command_get_status();
    break;

  case 0x02: {
    uint8_t minute = utility::bcd_to_dec(get_param());
    uint8_t second = utility::bcd_to_dec(get_param());
    uint8_t sector = utility::bcd_to_dec(get_param());

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
    return;
  }

  logic_transition(&core::logic_clearing_response, 1000);

#undef get_param
}

void core::logic_clearing_response() {
  response.clear();

  logic_transition(&core::logic_transferring_response, 1000);
}

void core::logic_transferring_response() {
  response.write(logic.response.read());

  if (logic.response.is_empty()) {
    logic_transition(&core::logic_deliver_interrupt, 1000);
  } else {
    logic_transition(&core::logic_transferring_response, 1000);
  }
}

void core::logic_deliver_interrupt() {
  if (interrupt_request == 0) {
    interrupt_request = logic.interrupt_request;

    logic_transition(&core::logic_idling, 1);
  } else {
    logic_transition(&core::logic_deliver_interrupt, 1);
  }
}

// -=====-
//  Drive
// -=====-

void core::drive_transition(stage_t stage, int timer) {
  drive.stage = stage;
  drive.timer = timer;
}

void core::drive_idling() {
}

void core::drive_getting_id() {
  if (interrupt_request == 0) {
    // INT2(02h,00h, 20h,00h, 53h,43h,45h,4xh)

    logic.response.write(0x02);
    logic.response.write(0x00);

    logic.response.write(0x20);
    logic.response.write(0x00);

    logic.response.write('S');
    logic.response.write('C');
    logic.response.write('E');
    logic.response.write('A');
    logic.interrupt_request = 2;

    drive_transition(&core::drive_idling, 1000);
    logic_transition(&core::logic_clearing_response, 1000);
  } else {
    drive_transition(&core::drive_getting_id, 1000);
  }
}

void core::drive_int2() {
  if (interrupt_request == 0) {
    logic.response.write(get_status_byte());
    logic.interrupt_request = 2;

    drive_transition(&core::drive_idling, 1000);
    logic_transition(&core::logic_clearing_response, 1000);
  } else {
    drive_transition(&core::drive_int2, 1000);
  }
}

void core::drive_reading() {
  logic.response.write(core::get_status_byte());
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

  drive_transition(&core::drive_reading, cycles);
  logic_transition(&core::logic_clearing_response, 1000);
}
