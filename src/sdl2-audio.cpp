#include "sdl2-audio.hpp"

#include <cassert>

using namespace psx;

constexpr int sample_rate = 44100;
constexpr int sample_size = sample_rate / 60;

sdl2_audio_t::sdl2_audio_t() {
  SDL_Init(SDL_INIT_AUDIO);

  want = SDL_AudioSpec {};
  want.freq = sample_rate;
  want.format = AUDIO_S16;
  want.samples = sample_size;
  want.callback = nullptr;
  want.padding = 0;
  want.channels = 1;

  device_id = SDL_OpenAudioDevice(nullptr, 0, &want, &have, SDL_AUDIO_ALLOW_ANY_CHANGE);

  if (device_id == 0) {
    printf("%s\n", SDL_GetError());
  }

  SDL_PauseAudioDevice(device_id, 0);
}

sdl2_audio_t::~sdl2_audio_t() {
  SDL_CloseAudioDevice(device_id);
}

bool sdl2_audio_t::render(int16_t *sound_buffer, int len) {
  return SDL_QueueAudio(device_id, sound_buffer, len * sizeof(int16_t)) == 0;
}
