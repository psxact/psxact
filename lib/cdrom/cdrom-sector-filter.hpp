#ifndef CDROM_CDROM_SECTOR_FILTER_HPP_
#define CDROM_CDROM_SECTOR_FILTER_HPP_

#include <cstdint>
#include "cdrom/cdrom-sector.hpp"

namespace psx::cdrom {

  class cdrom_sector_filter_t {
    uint8_t file {0};
    uint8_t channel {0};

  public:
    void put_file(uint8_t val);
    void put_channel(uint8_t val);
    bool match(const cdrom_sector_t &sector) const;
  };
}  // namespace psx::cdrom

#endif  // CDROM_CDROM_SECTOR_FILTER_HPP_
