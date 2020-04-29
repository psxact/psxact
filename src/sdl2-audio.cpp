#include "sdl2-audio.hpp"

using namespace psx;

constexpr int sample_rate = 44100;
constexpr int sample_size = sample_rate / 60;

sdl2_audio_t::sdl2_audio_t()
  : device_id()
  , want()
  , have() {

  want.freq = sample_rate;
  want.format = AUDIO_S16;
  want.samples = sample_size;
  want.callback = nullptr;
  want.padding = 0;
  want.channels = 2;

  device_id = SDL_OpenAudioDevice(nullptr, 0, &want, &have, SDL_AUDIO_ALLOW_ANY_CHANGE);

  if (device_id == 0) {
    printf("%s\n", SDL_GetError());
  }

  SDL_PauseAudioDevice(device_id, 0);
}

sdl2_audio_t::~sdl2_audio_t() {
  SDL_CloseAudioDevice(device_id);
}

bool sdl2_audio_t::render(psx::output_params_audio_t &params) {
  return SDL_QueueAudio(device_id, params.buffer, params.buffer_len * sizeof(int16_t)) == 0;
}
