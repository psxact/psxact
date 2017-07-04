#ifndef __PSXACT_CDROM_DRIVE_HPP__
#define __PSXACT_CDROM_DRIVE_HPP__

#include <cstdint>
#include "../fifo.hpp"
#include "../state.hpp"

namespace cdrom {
  void initialize(cdrom_state_t *state, const std::string &game_file_name);

  uint32_t io_read(cdrom_state_t *state, int width, uint32_t address);

  void io_write(cdrom_state_t *state, int width, uint32_t address, uint32_t data);

  void tick(cdrom_state_t *state);

  uint8_t get_status_byte(cdrom_state_t *state);

  void read_sector(cdrom_state_t *state);

  namespace command {
    void get_id(cdrom_state_t *state);
    void get_status(cdrom_state_t *state);
    void init(cdrom_state_t *state);
    void pause(cdrom_state_t *state);
    void read_n(cdrom_state_t *state);
    void read_table_of_contents(cdrom_state_t *state);
    void seek_data_mode(cdrom_state_t *state);
    void set_mode(cdrom_state_t *state, uint8_t mode);
    void set_seek_target(cdrom_state_t *state, uint8_t minute, uint8_t second, uint8_t sector);
    void test(cdrom_state_t *state, uint8_t function);
    void unmute(cdrom_state_t *state);
  }

  namespace logic {
    void transition(cdrom_state_t *state, cdrom_state_t::stage_t stage, int timer);

    void idling(cdrom_state_t *state);
    void transferring_parameters(cdrom_state_t *state);
    void transferring_command(cdrom_state_t *state);
    void executing_command(cdrom_state_t *state);
    void clearing_response(cdrom_state_t *state);
    void transferring_response(cdrom_state_t *state);
    void deliver_interrupt(cdrom_state_t *state);
  }

  namespace drive {
    void transition(cdrom_state_t *state, cdrom_state_t::stage_t stage, int timer);

    void idling(cdrom_state_t *state);
    void int2(cdrom_state_t *state);
    void getting_id(cdrom_state_t *state);
    void reading(cdrom_state_t *state);
  }
}

#endif // __PSXACT_CDROM_DRIVE_HPP__
