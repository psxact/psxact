#include "spu/adpcm.hpp"

using namespace psx::spu;

adpcm_header_t adpcm_header_t::create(uint16_t header) {
  adpcm_header_t result;
  result.shift = header & 15;
  result.filter = (header >> 4) & 15;
  result.loop_end = (header & (1 << 8)) != 0;
  result.loop_repeat = (header & (1 << 9)) != 0;
  result.loop_start = (header & (1 << 10)) != 0;

  return result;
}

adpcm_sample_t adpcm_sample_t::create(uint16_t sample) {
  adpcm_sample_t result;
  result.coded[0] = int16_t((sample << 12) & 0xf000);
  result.coded[1] = int16_t((sample <<  8) & 0xf000);
  result.coded[2] = int16_t((sample <<  4) & 0xf000);
  result.coded[3] = int16_t((sample <<  0) & 0xf000);

  return result;
}
