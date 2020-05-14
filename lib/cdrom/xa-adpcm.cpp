#include "cdrom/xa-adpcm.hpp"

#include <cassert>
#include "util/int.hpp"

using namespace psx::cdrom;
using namespace psx::util;

static const int pos_xa_adpcm_table[5] = {0, +60, +115, +98, +122};
static const int neg_xa_adpcm_table[5] = {0,   0,  -52, -55,  -60};

std::tuple<int16_t, int16_t> xa_adpcm_t::read() {
  return std::tuple(left.get_output(), right.get_output());
}

void xa_adpcm_t::decode(const cdrom_sector_t &sector) {
  for (int segment = 0; segment < 18; segment++) {
    decode_segment(sector, segment);
  }
}

void xa_adpcm_t::decode_segment(const cdrom_sector_t &sector, int segment) {
  auto coding_info = sector.get_xa_coding_info();
  auto is_stereo   = ((coding_info >> 0) & 3) == 1;
  auto is_18900hz  = ((coding_info >> 2) & 3) == 1;
  auto is_8bit     = ((coding_info >> 4) & 3) == 1;

  assert(is_18900hz == false);
  assert(is_8bit == false);

  auto head_base = 24 + (segment * 128) + 4; // skip sync,header,subheader
  auto data_base = 24 + (segment * 128) + 16;

  // The first 16 bytes are header data.
  // The next 112 bytes are sample data. The arrangement of this section depends on the stereo/depth bits.

  if (!is_8bit) {
    if (is_stereo) {
      for (int stream = 0; stream < 4; stream++) {
        uint8_t header_left = sector.get(head_base + (stream * 2) + 0);
        uint8_t header_right = sector.get(head_base + (stream * 2) + 1);

        for (int i = 0; i < 0x70; i += 4) {
          uint8_t sample = sector.get(data_base + stream + i);
          uint8_t sample_left = (sample << 4) & 0xf0;
          uint8_t sample_right = (sample << 0) & 0xf0;

          decode_sample_pair(header_left, sample_left, header_right, sample_right);
        }
      }
    }
  }
}

void xa_adpcm_t::decode_sample_pair(uint8_t left_header, uint8_t left_data, uint8_t right_header, uint8_t right_data) {
  left.put_sample(decode_sample(left_header, left_data, prev_samples_left));
  right.put_sample(decode_sample(right_header, right_data, prev_samples_right));
}

int16_t xa_adpcm_t::decode_sample(uint8_t header, uint8_t sample, int16_t (&prev)[2]) {
  int32_t shift = header & 15;
  int32_t filter = (header >> 4) & 3;

  int32_t f0 = pos_xa_adpcm_table[filter];
  int32_t f1 = neg_xa_adpcm_table[filter];
  int32_t t  = int16_t(sample << 8) >> shift;

  t += ((f0 * prev[0] + f1 * prev[1]) + 32) / 64;

  int32_t s  = int_t<16>::clamp(t);

  prev[1] = prev[0];
  prev[0] = s;
  
  return s;
}
