#include "cdrom/cdrom-sector-filter.hpp"

using namespace psx::cdrom;

void cdrom_sector_filter::put_file(uint8_t val) {
  file = val;
}

void cdrom_sector_filter::put_channel(uint8_t val) {
  channel = val;
}

bool cdrom_sector_filter::match(const cdrom_sector &sector) const {
  if (sector.get_type() != cdrom_sector_type::mode2_form2) {
    return false;
  }

  return sector.get_xa_file() == file
      && sector.get_xa_channel() == channel;
}
