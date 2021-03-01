#include "cdrom/cdrom-sector.hpp"

#include "util/bcd.hpp"
#include "util/unused.hpp"
#include "util/panic.hpp"

using namespace psx::cdrom;
using namespace psx::util;

enum {
  MINUTE_OFFSET = 12,
  SECOND_OFFSET = 13,
  SECTOR_OFFSET = 14,
  MODE_OFFSET = 15,
  XA_FILE_OFFSET = 16,
  XA_CHANNEL_OFFSET = 17,
  XA_SUB_MODE_OFFSET = 18,
  XA_CODING_INFO_OFFSET = 19
};

uint8_t cdrom_sector::get_minute() const {
  return buffer[MINUTE_OFFSET];
}

uint8_t cdrom_sector::get_second() const {
  return buffer[SECOND_OFFSET];
}

uint8_t cdrom_sector::get_sector() const {
  return buffer[SECTOR_OFFSET];
}

uint8_t cdrom_sector::get_mode() const {
  return buffer[MODE_OFFSET];
}

uint8_t cdrom_sector::get_xa_file() const {
  return buffer[XA_FILE_OFFSET];
}

uint8_t cdrom_sector::get_xa_channel() const {
  return buffer[XA_CHANNEL_OFFSET];
}

uint8_t cdrom_sector::get_xa_sub_mode() const {
  return buffer[XA_SUB_MODE_OFFSET];
}

uint8_t cdrom_sector::get_xa_coding_info() const {
  return buffer[XA_CODING_INFO_OFFSET];
}

cdrom_sector_type cdrom_sector::get_type() const {
  switch (get_mode()) {
    case 0: return cdrom_sector_type::mode0;
    case 1: return cdrom_sector_type::mode1;
    case 2:
      return (get_xa_sub_mode() & 0x20)
          ? cdrom_sector_type::mode2_form2
          : cdrom_sector_type::mode2_form1;
  }

  return cdrom_sector_type::unknown;
}

void cdrom_sector::fill_from(FILE *file, cdrom_timecode timecode) {
  constexpr int sectors_per_second = 75;
  constexpr int seconds_per_minute = 60;
  constexpr int sectors_per_minute = sectors_per_second * seconds_per_minute;

  auto leadin = 2 * sectors_per_second;
  auto sector =
      (timecode.minute * sectors_per_minute) +
      (timecode.second * sectors_per_second) + timecode.sector;

	auto seek_result = fseek(file, (sector - leadin) * CDROM_SECTOR_SIZE, SEEK_SET);
	MAYBE_UNUSED(seek_result);

  PANIC_IF(seek_result != 0,
		"unable to seek sector %d", sector);

	auto read_result = fread(buffer, sizeof(uint8_t), CDROM_SECTOR_SIZE, file);
	MAYBE_UNUSED(read_result);

  PANIC_IF(read_result != CDROM_SECTOR_SIZE,
		"unable to read sector %d", sector);

  // Sanity check to make sure we've read the correct sector.

  PANIC_IF(
    bcd::to_dec(get_minute()) != timecode.minute ||
    bcd::to_dec(get_second()) != timecode.second ||
    bcd::to_dec(get_sector()) != timecode.sector, "");
}

uint8_t cdrom_sector::get(int index) const {
  PANIC_IF(index >= CDROM_SECTOR_SIZE,
		"unable to get data at %d of sector", index);

  return buffer[index];
}
