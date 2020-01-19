#ifndef CDROM_CORE_HPP_
#define CDROM_CORE_HPP_

#include <cstdint>
#include <cstdio>
#include <string>
#include "util/fifo.hpp"
#include "interrupt-access.hpp"
#include "memory-component.hpp"

using namespace psx::util;

namespace psx::cdrom {

struct sector_timecode_t {
  uint8_t minute;
  uint8_t second;
  uint8_t sector;
};

class core_t : public memory_component_t {
  interrupt_access_t *irq;

  int32_t index;
  int32_t interrupt_enable;
  int32_t interrupt_request;
  int32_t interrupt_timer;

  sector_timecode_t seek_timecode;
  sector_timecode_t read_timecode;
  bool seek_unprocessed;

  fifo_t<uint8_t, 4> parameter_fifo;
  fifo_t<uint8_t, 4> response_fifo;
  fifo_t<uint8_t, 12> data_fifo;
  uint8_t data_buffer[4096];

  uint8_t command;
  bool command_unprocessed;
  bool busy;
  bool is_seeking;
  bool is_reading;

  std::string game_file_name;
  FILE *game_file;

  typedef void (core_t:: *stage_t)();

  struct {
    stage_t stage;
    int32_t timer;

    int32_t interrupt_request;

    fifo_t<uint8_t, 4> parameter_fifo;
    fifo_t<uint8_t, 4> response_fifo;
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

 public:
  core_t(interrupt_access_t *irq, const char *game_file_name, bool log_enabled);

  uint8_t io_read_byte(uint32_t address);

  uint32_t io_read_word(uint32_t address);

  void io_write_byte(uint32_t address, uint8_t data);

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

  void tick(int amount);

  void do_seek();

  int32_t get_cycles_per_sector();

  uint8_t get_status_byte();

  int32_t get_read_cursor();

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

}  // namespace psx::cdrom

#endif  // CDROM_CORE_HPP_
