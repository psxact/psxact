#ifndef __PSXACT_CDROM_DRIVE_HPP__
#define __PSXACT_CDROM_DRIVE_HPP__

#include <cstdint>
#include <cstdio>
#include <string>
#include "../bus.hpp"
#include "../fifo.hpp"

struct cdrom_sector_timecode_t {
  uint8_t minute;
  uint8_t second;
  uint8_t sector;
};

struct cdrom_core {
  int32_t index;
  int32_t interrupt_enable;
  int32_t interrupt_request;

  cdrom_sector_timecode_t seek_timecode;
  cdrom_sector_timecode_t read_timecode;
  bool seek_unprocessed;

  fifo_t<uint8_t, 4> parameter;
  fifo_t<uint8_t, 4> response;
  fifo_t<uint8_t, 12> data;
  uint8_t data_buffer[4096];

  uint8_t command;
  bool command_is_new;
  bool busy;

  std::string game_file_name;
  FILE *game_file;

  typedef void (cdrom_core::*stage_t)();

  struct {
    stage_t stage;
    int32_t timer;

    int32_t interrupt_request;

    fifo_t<uint8_t, 4> parameter;
    fifo_t<uint8_t, 4> response;
    uint8_t command;
  } logic;

  struct {
    stage_t stage;
    int32_t timer;
  } drive;

  struct {
    bool double_speed;
    bool read_whole_sector;
  } mode;

  uint32_t io_read(bus_width_t width, uint32_t address);

  uint8_t io_read_port_0();

  uint8_t io_read_port_1();

  uint8_t io_read_port_2();

  uint8_t io_read_port_3();

  uint8_t io_read_internal(uint32_t port);

  void io_write(bus_width_t width, uint32_t address, uint32_t data);

  void io_write_port_0_n(uint8_t data);

  void io_write_port_1_0(uint8_t data);

  void io_write_port_1_1(uint8_t data);

  void io_write_port_1_2(uint8_t data);

  void io_write_port_1_3(uint8_t data);

  void io_write_port_2_0(uint8_t data);

  void io_write_port_2_1(uint8_t data);

  void io_write_port_2_2(uint8_t data);

  void io_write_port_2_3(uint8_t data);

  void io_write_port_3_0(uint8_t data);

  void io_write_port_3_1(uint8_t data);

  void io_write_port_3_2(uint8_t data);

  void io_write_port_3_3(uint8_t data);

  void init(const char *game_file_name);

  void tick();

  void do_seek();

  int get_cycles_per_sector();

  uint8_t get_status_byte();

  void read_sector();

  void command_get_id();

  void command_get_status();

  void command_init();

  void command_pause();

  void command_read_n();

  void command_read_table_of_contents();

  void command_seek_data_mode();

  void command_set_mode(uint8_t mode);

  void command_set_seek_target(uint8_t minute, uint8_t second, uint8_t sector);

  void command_test(uint8_t function);

  void command_unmute();

  void logic_transition(stage_t stage, int timer);

  void logic_idling();

  void logic_transferring_parameters();

  void logic_transferring_command();

  void logic_executing_command();

  void logic_clearing_response();

  void logic_transferring_response();

  void logic_deliver_interrupt();

  void drive_transition(stage_t stage, int timer);

  void drive_idling();

  void drive_int2();

  void drive_getting_id();

  void drive_reading();
};

#endif // __PSXACT_CDROM_DRIVE_HPP__
