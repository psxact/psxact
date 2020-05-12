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

  enum class cdrom_sector_type_t {
    /// Used when a determination couldn't be made.
    unknown,
    /// Empty, used for padding?
    mode0,
    /// CD-ROM, used for data.
    mode1,
    /// CD-XA Form 1, used for data.
    mode2_form1,
    /// CD-XA Form 2, used for XA-ADPCM.
    mode2_form2
  };

  class cdrom_sector_t {
    uint8_t buffer[CDROM_SECTOR_SIZE];

  public:
    /// Returns the uninterpreted minute marker from the current data.
    uint8_t get_minute() const;

    /// Returns the uninterpreted second marker from the current data.
    uint8_t get_second() const;

    /// Returns the uninterpreted sector marker from the current data.
    uint8_t get_sector() const;

    /// Returns the uninterpreted mode marker from the current data.
    uint8_t get_mode() const;

    /// Returns the uninterpreted CD-XA file marker from the current data.
    uint8_t get_xa_file() const;

    /// Returns the uninterpreted CD-XA channel marker from the current data.
    uint8_t get_xa_channel() const;

    /// Returns the uninterpreted CD-XA sub-mode marker from the current data.
    uint8_t get_xa_sub_mode() const;

    /// Returns the uninterpreted CD-XA coding-info marker from the current data.
    uint8_t get_xa_coding_info() const;

    /// Returns the sector type for the currently loaded data.
    cdrom_sector_type_t get_type() const;

    /// Reads a sector from the specified file, at the specified timecode.
    void fill_from(FILE *file, cdrom_timecode_t timecode);

    uint8_t get(int index) const;
  };
}  // namespace psx::cdrom

#endif  // CDROM_SECTOR_HPP_
