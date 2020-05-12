#ifndef CDROM_CDROM_MODE_HPP_
#define CDROM_CDROM_MODE_HPP_

#include <cstdint>

namespace psx::cdrom {
  class cdrom_mode_t {
    uint8_t val;

  public:
    explicit cdrom_mode_t(uint8_t val) : val(val) {}

    bool double_speed() const;
    bool send_xa_adpcm_to_spu() const;
    bool read_whole_sector() const;
    bool filter_xa_adpcm() const;
    bool send_audio_reports() const;
    bool auto_pause_audio() const;
    bool read_cd_da() const;
  };
}

#endif  // CDROM_CDROM_MODE_HPP_
