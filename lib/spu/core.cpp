#include "spu/core.hpp"

#include <cassert>
#include "util/int.hpp"
#include "util/uint.hpp"
#include "args.hpp"
#include "timing.hpp"

using namespace psx::spu;
using namespace psx::util;

core_t::core_t()
  : addressable_t("spu", args::log_spu) {
}

core_t::~core_t() {
}

uint16_t core_t::get_register(register_t reg) {
  return registers[uint32_t(reg) - 0x1f801c00];
}

void core_t::put_register(register_t reg, uint16_t val) {
  registers[uint32_t(reg) - 0x1f801c00] = val;
}

void core_t::put_status_register() {
  uint16_t status = 0;
  
  status |= get_register(register_t::control) & 0x3f;
  status |= (capture_address & 0x100) << 3;

  put_register(register_t::status, status);
}

void core_t::run(int amount) {
  prescaler += amount;

  while (prescaler >= SPU_DIVIDER) {
    prescaler -= SPU_DIVIDER;
    tick();
  }
}

void core_t::tick() {
  int lsample = 0;
  int rsample = 0;

  for (int v = 0; v < 24; v++) {
    int32_t l, r;

    voice_tick(v, &l, &r);

    lsample += l;
    rsample += r;
  }

  key_on = 0;
  key_off = 0;

  sample_buffer[sample_buffer_index & 0x7ff] = int_t<16>::clamp(lsample);
  sample_buffer_index++;
  sample_buffer[sample_buffer_index & 0x7ff] = int_t<16>::clamp(rsample);
  sample_buffer_index++;

  capture_address = (capture_address + 1) & 0x1ff;

  // Update registers
  put_status_register();
  put_register(register_t::endx_lo, uint16_t(endx >> 0));
  put_register(register_t::endx_hi, uint16_t(endx >> 16));
}

void core_t::voice_tick(int v, int32_t *l, int32_t *r) {
  auto &voice = voices[v];
  voice_decoder_tick(v);

  int32_t sample = voice.apply_envelope(voice.raw_sample());

  if (v == 1) {
    ram.write(0x400 | capture_address, sample);
  }

  if (v == 3) {
    ram.write(0x600 | capture_address, sample);
  }

  *l = (sample * voice.volume_left.get_level()) >> 15;
  *r = (sample * voice.volume_right.get_level()) >> 15;

  if (voice.start_delay > 0) {
    voice.start_delay--;
  } else {
    voice.adsr.step();
    voice.counter_step();
  }

  int32_t mask = 1 << v;

  if (key_off & mask) {
    key_off &= ~mask;
    voice.adsr.key_off();
  }

  if (key_on & mask) {
    key_on &= ~mask;
    endx &= ~mask;
    voice.adsr.key_on();
    voice.start_delay = 4;
    voice.phase = 0;
    voice.header = adpcm_header_t::create(0);
    voice.decoder_fifo.clear();
    voice.last_samples[0] = 0;
    voice.last_samples[1] = 0;
    voice.current_address = voice.start_address & ~7;
  }
}

void core_t::voice_decoder_tick(int v) {
  auto &voice = voices[v];

  if (voice.decoder_fifo.size() >= 11) {
    return;
  }

  if ((voice.current_address & 7) == 0) {
    if (voice.header.loop_end) {
      voice.current_address = voice.loop_address & ~7;
      endx |= (1 << v);

      if (!voice.header.loop_repeat) {
        voice.adsr.key_off();
        voice.adsr.put_level(0);
      }
    }

    voice.put_header(ram.read(voice.current_address));
    voice.current_address++;
  }

  voice.put_sample(ram.read(voice.current_address));
  voice.current_address++;
}

int16_t* core_t::get_sample_buffer() const {
  return (int16_t *) sample_buffer;
}

uint32_t core_t::get_sample_buffer_index() const {
  return sample_buffer_index;
}

void core_t::reset_sample() {
  sample_buffer_index = 0;
}