#include "spu/voice.hpp"

#include "spu/gauss.hpp"

using namespace psx::spu;
using namespace psx::util;

static const int pos_xa_adpcm_table[16] = {
    0,  60, 115,  98,
  122,   0,   0,   0,
    0,   0,   0,   0,
    0,   0,   0,   0
};

static const int neg_xa_adpcm_table[16] = {
    0,   0, -52, -55,
  -60,   0,   0,   0,
    0,   0,   0,   0,
    0,   0,   0,   0
};

void voice_t::put_header(uint16_t val) {
  header = adpcm_header_t::create(val);

  if (header.loop_start) {
    loop_address = current_address & ~7;
  }
}

void voice_t::put_sample(uint16_t val) {
  adpcm_sample_t sample = adpcm_sample_t::create(val);

  // decode
  int wp = pos_xa_adpcm_table[header.filter];
  int wn = neg_xa_adpcm_table[header.filter];
  int shift = header.shift;

  if (shift > 12) {
    val &= 0x8888;
    shift = 8;
  }

  for (int16_t coded : sample.coded) {
    int32_t out = int32_t(coded) >> shift;
    out += (int32_t(last_samples[0]) * wp) >> 6;
    out += (int32_t(last_samples[1]) * wn) >> 6;
    out  = int_t<16>::clamp(out);

    decoder_fifo.write(int16_t(out));

    last_samples[1] = last_samples[0];
    last_samples[0] = out;
  }
}

int32_t voice_t::raw_sample() {
  int index = (phase >> 4) & 0xff;

  return gauss_t::filter(index,
    decoder_fifo.at(0),
    decoder_fifo.at(1),
    decoder_fifo.at(2),
    decoder_fifo.at(3));
}

int32_t voice_t::apply_envelope(int32_t raw) {
  int32_t level = int32_t(adsr.get_level());

  return (level * raw) >> 15;
}

void voice_t::counter_step() {
  uint16_t r = pitch;

  // TODO: pitch modulation

  if (r > 0x3fff) {
    r = 0x3fff;
  }

  uint32_t p = phase + r;
  uint32_t s = p >> 12;

  phase = p & 0xfff;
  decoder_fifo.discard(s);
}
