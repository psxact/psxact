#include "spu/core.hpp"

#include <cassert>
#include <SDL2/SDL.h>
#include "util/int.hpp"
#include "util/uint.hpp"

using namespace psx::spu;
using namespace psx::util;

static void audio_callback(void *userdata, uint8_t *stream, int len) {
  auto spu = ((psx::spu::core_t *) userdata)->get_sample();
  auto ptr = (int16_t *) stream;

  for (int i = 0; i < len; i += 2) {
    *ptr = *spu;
    ptr++;
    spu++;
  }
}

core_t::core_t(bool log_enabled)
  : addressable_t("spu", log_enabled) {

  SDL_AudioSpec want;
  SDL_AudioSpec have;

  want.callback = audio_callback;
  want.channels = 2;
  want.format = AUDIO_S16;
  want.freq = 44100;
  want.samples = 1024;
  want.userdata = this;

  SDL_OpenAudio(&want, &have);
  SDL_PauseAudio(0);
}

core_t::~core_t() {
  SDL_CloseAudio();
}

uint16_t core_t::get_register(register_t reg) {
  return registers[uint32_t(reg) - 0x1f801c00];
}

void core_t::put_register(register_t reg, uint16_t val) {
  registers[uint32_t(reg) - 0x1f801c00] = val;
}

void core_t::run(int amount) {
  prescaler += amount;

  while (prescaler >= 0x300) {
    prescaler -= 0x300;
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

  sample_buffer[sample_buffer_index] = int_t<16>::clamp(lsample);
  sample_buffer_index = (sample_buffer_index + 1) & 2047;
  sample_buffer[sample_buffer_index] = int_t<16>::clamp(rsample);
  sample_buffer_index = (sample_buffer_index + 1) & 2047;

  // update ENDX
  put_register(register_t::endx_lo, uint16_t(endx >> 0));
  put_register(register_t::endx_hi, uint16_t(endx >> 16));
}

void core_t::voice_tick(int v, int32_t *l, int32_t *r) {
  auto &voice = voices[v];
  if (voice.start_delay > 0) {
    voice.start_delay--;
    return;
  }

  voice_decoder_tick(v);

  int32_t raw = voice.raw_sample();
  int32_t sample = raw; // TODO: apply ADSR

  // if (v == 1) {
  //   sound_ram.write(0x400, sample);
  // }

  // if (v == 3) {
  //   sound_ram.write(0x600, sample);
  // }

  int32_t left = voice.volume_left.apply(sample);
  int32_t right = voice.volume_right.apply(sample);

  voice.counter_step();

  if (key_on & (1 << v)) {
    endx &= ~(1 << v);
    voice.start_delay = 4;
    voice.phase = 0;
    voice.header = adpcm_header_t::create(0);
    voice.decoder_fifo.clear();
    voice.last_samples[0] = 0;
    voice.last_samples[1] = 0;
    voice.current_address = voice.start_address & ~7;
  }

  *l = left;
  *r = right;
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
    }

    uint16_t header = ram.read(voice.current_address);

    if (voice.current_address == 0x820 && header == 0) {
      log("!! read bogus value !!");
    }

    voice.put_header(header);
    voice.current_address++;
  }

  voice.put_sample(ram.read(voice.current_address));
  voice.current_address++;
}

const int16_t* core_t::get_sample() const {
  return sample_buffer;
}
