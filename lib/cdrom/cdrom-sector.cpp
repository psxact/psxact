#include "cdrom/cdrom-sector.hpp"

#include <cassert>
#include "util/bcd.hpp"

using namespace psx::cdrom;
using namespace psx::util;

uint8_t cdrom_sector_t::get_minute() const {
  return buffer[12];
}

uint8_t cdrom_sector_t::get_second() const {
  return buffer[13];
}

uint8_t cdrom_sector_t::get_sector() const {
  return buffer[14];
}

bool cdrom_sector_t::is_mode_0() const {
  return buffer[15] == 0;
}

bool cdrom_sector_t::is_mode_1() const {
  return buffer[15] == 1;
}

bool cdrom_sector_t::is_mode_2_form_1() const {
  return buffer[15] == 2 && (buffer[18] & 0x20) == 0;
}

bool cdrom_sector_t::is_mode_2_form_2() const {
  return buffer[15] == 2 && (buffer[18] & 0x20) != 0;
}

void cdrom_sector_t::fill_from(FILE *file, cdrom_timecode_t timecode) {
  constexpr int sectors_per_second = 75;
  constexpr int seconds_per_minute = 60;
  constexpr int sectors_per_minute = sectors_per_second * seconds_per_minute;

  auto leadin = 2 * sectors_per_second;
  auto sector =
      (timecode.minute * sectors_per_minute) +
      (timecode.second * sectors_per_second) + timecode.sector;

  fseek(file, (sector - leadin) * CDROM_SECTOR_SIZE, SEEK_SET);
  fread(buffer, sizeof(uint8_t), CDROM_SECTOR_SIZE, file);

  // Sanity check to make sure we've read the correct sector.

  assert(
    bcd::to_dec(buffer[12]) == timecode.minute &&
    bcd::to_dec(buffer[13]) == timecode.second &&
    bcd::to_dec(buffer[14]) == timecode.sector);
}

uint8_t cdrom_sector_t::get(int index) const {
  assert(index < CDROM_SECTOR_SIZE);
  return buffer[index];
}
