#include "cdrom/cdrom-mode.hpp"

using namespace psx::cdrom;

bool cdrom_mode::double_speed() const {
  return (val & 0x80) != 0;
}

bool cdrom_mode::send_xa_adpcm_to_spu() const {
  return (val & 0x40) != 0;
}

bool cdrom_mode::read_whole_sector() const {
  return (val & 0x20) != 0;
}

bool cdrom_mode::filter_xa_adpcm() const {
  return (val & 0x08) != 0;
}

bool cdrom_mode::send_audio_reports() const {
  return (val & 0x04) != 0;
}

bool cdrom_mode::auto_pause_audio() const {
  return (val & 0x02) != 0;
}

bool cdrom_mode::read_cd_da() const {
  return (val & 0x01) != 0;
}
