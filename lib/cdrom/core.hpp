#ifndef CDROM_CORE_HPP_
#define CDROM_CORE_HPP_

#include <cstdint>
#include <optional>
#include "cdrom/cdrom-sector.hpp"
#include "util/fifo.hpp"
#include "util/wire.hpp"
#include "addressable.hpp"
#include "dma-comms.hpp"

namespace psx::cdrom {

  enum class cdrom_drive_state_t {
    idle,
    reading,
    seeking,
    playing
  };

  class core_t final
      : public addressable_t
      , public dma_comms_t {
    util::wire_t irq;

    cdrom_drive_state_t drive_state {};
    cdrom_sector_t sector {};
    int32_t sector_read_cursor {};
    int32_t sector_read_offset {};
    int32_t sector_read_length {};
    bool sector_read_active {};

    cdrom_timecode_t seek_timecode {};
    cdrom_timecode_t read_timecode {};
    bool seek_pending {};

    util::fifo_t<uint8_t, 4> parameter {};
    util::fifo_t<uint8_t, 4> response {};
    std::optional<uint8_t> command {};
    uint8_t index {};
    uint8_t irq_flag {};
    uint8_t irq_mask {};
    uint8_t mode {};
    int timer {};

    int int1_timer;
    void (cdrom::core_t:: *int1)();

    int int2_timer;
    void (cdrom::core_t:: *int2)();

    std::optional<FILE *> disc_file;

  public:
    core_t(util::wire_t irq, const char *game_file_name);

    void tick(int amount);

    /// Called internally to (try to) calculate the length of a seek operation.
    int get_seek_time() const;

    /// Called internally to (try to) calculate the length of a read operation.
    int get_read_time() const;

    /// Called on reads from the data register.
    uint8_t get_data();

    /// Called internally to deliver drive status.
    uint8_t get_drive_status();

    /// Called on reads from the IRQ flag register.
    uint8_t get_irq_flag() const;

    /// Called on reads from the IRQ mask register.
    uint8_t get_irq_mask() const;

    /// Called internally to read parameters.
    uint8_t get_parameter();

    /// Called on reads from the response register.
    uint8_t get_response();

    /// Called on reads from the status register.
    uint8_t get_status();

    /// Called on writes to the IRQ flag register.
    void ack_irq_flag(uint8_t val);

    /// Called on writes to the command register.
    void put_command(uint8_t val);

    /// Called on writes to the host control register.
    void put_host_control(uint8_t val);

    /// Called internally to deliver IRQs.
    void put_irq_flag(uint8_t val);

    /// Called on writes to the IRQ mask register.
    void put_irq_mask(uint8_t val);

    /// Called on writes to the parameter register.
    void put_parameter(uint8_t val);

    /// Called internally to deliver responses.
    void put_response(uint8_t val);

    /// Called continuously after processing a 'ReadN' command, to deliver sectors.
    void int1_read_n();

    /// Called after processing a 'GetID' command, to deliver the 2nd response.
    void int2_get_id();

    /// Called after processing a 'Init' command, to deliver the 2nd response.
    void int2_init();

    /// Called after processing a 'Pause' command, to deliver the 2nd response.
    void int2_pause();

    /// Called after processing a 'ReadTOC' command, to deliver the 2nd response.
    void int2_read_toc();

    /// Called after processing a 'SeekL' command, to deliver the 2nd response.
    void int2_seek_l();

    int dma_speed() override;
    bool dma_read_ready() override;
    bool dma_write_ready() override;
    uint32_t dma_read() override;
    void dma_write(uint32_t val) override;

    uint32_t io_read(address_width_t width, uint32_t address) override;
    void io_write(address_width_t width, uint32_t address, uint32_t data) override;
  };
}  // namespace psx::cdrom

#endif  // CDROM_CORE_HPP_
