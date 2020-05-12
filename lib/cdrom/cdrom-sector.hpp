#ifndef CDROM_SECTOR_HPP_
#define CDROM_SECTOR_HPP_

#include <cstdint>
#include <cstdio>

namespace psx::cdrom {
  constexpr int CDROM_SECTOR_SIZE = 0x930;

  struct cdrom_timecode_t {
    uint8_t minute;
    uint8_t second;
    uint8_t sector;
  };

  class cdrom_sector_t {
    uint8_t buffer[CDROM_SECTOR_SIZE];

  public:
    /// Returns the minute marker from the current data.
    uint8_t get_minute() const;

    /// Returns the second marker from the current data.
    uint8_t get_second() const;

    /// Returns the sector marker from the current data.
    uint8_t get_sector() const;

    /// Returns true if the current data represents a CD Mode 0 sector.
    bool is_mode_0() const;

    /// Returns true if the current data represents a CD Mode 1 sector.
    bool is_mode_1() const;

    /// Returns true if the current data represents a CD-XA Mode 2 Form 1 sector.
    bool is_mode_2_form_1() const;

    /// Returns true if the current data represents a CD-XA Mode 2 Form 2 sector.
    bool is_mode_2_form_2() const;

    /// Reads a sector from the specified file, at the specified timecode.
    void fill_from(FILE *file, cdrom_timecode_t timecode);

    uint8_t get(int index) const;
  };
} // namespace psx::cdrom

#endif // CDROM_SECTOR_HPP_
