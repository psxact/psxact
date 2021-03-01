#include "cdrom/xa-adpcm-decoder.hpp"

#include "util/int.hpp"
#include "util/unused.hpp"
#include "util/panic.hpp"

using namespace psx::cdrom;
using namespace psx::util;

static const int pos_xa_adpcm_table[5] = {0, +60, +115, +98, +122};
static const int neg_xa_adpcm_table[5] = {0,   0,  -52, -55,  -60};

std::tuple<int16_t, int16_t> xa_adpcm_decoder::read() {
  return std::tuple(
    channel[0].get_output(),
    channel[1].get_output());
}

void xa_adpcm_decoder::decode(const cdrom_sector &sector) {
  for (int segment = 0; segment < 18; segment++) {
    decode_segment(sector, segment);
  }
}

void xa_adpcm_decoder::decode_segment(const cdrom_sector &sector, int segment) {
  auto coding_info = sector.get_xa_coding_info();
  auto is_stereo   = ((coding_info >> 0) & 3) == 1;
  auto is_18900hz  = ((coding_info >> 2) & 3) == 1;
  auto is_8bit     = ((coding_info >> 4) & 3) == 1;

	MAYBE_UNUSED(is_18900hz);

  PANIC_IF(is_18900hz, "18.9khz playback isn't supported");
  PANIC_IF(is_8bit, "8-bit playback isn't supported");

  auto base = 24 + (segment * 128);
  auto step = is_stereo ? 2 : 1;

  auto read_header = [&](int n) { return sector.get(base + n + 4); };
  auto read_sample = [&](int n) { return sector.get(base + n + 16); };

  if (!is_8bit) {
    for (int stream = 0; stream < 8; stream += step) {
      for (int nibble = 0; nibble < 0x70; nibble += 4) {
        if (is_stereo) {
          auto header1 = read_header(stream + 0);
          auto header2 = read_header(stream + 1);
          auto sample  = read_sample((stream / 2) + nibble);

          decode_sample(header1, int16_t((sample << 12) & 0xf000), 0);
          decode_sample(header2, int16_t((sample <<  8) & 0xf000), 1);
        } else {
          auto header = read_header(stream);
          auto sample = read_sample((stream / 2) + nibble);
          auto shift  = (stream & 1) ? 8 : 12;

          decode_sample(header, int16_t((sample << shift) & 0xf000), 0);
          decode_sample(header, int16_t((sample << shift) & 0xf000), 1);
        }
      }
    }
  }
}

void xa_adpcm_decoder::decode_sample(uint8_t header, int16_t sample, int32_t c) {
  int32_t shift = header & 15;
  int32_t filter = (header >> 4) & 3;

  int32_t pos = pos_xa_adpcm_table[filter];
  int32_t neg = neg_xa_adpcm_table[filter];
  int32_t val = sample >> shift;

  val += ((pos * prev[c][0]) + (neg * prev[c][1])) >> 6;
  val  = int_t<16>::clamp(val);

  prev[c][1] = prev[c][0];
  prev[c][0] = val;

  channel[c].put_sample(val);
}
