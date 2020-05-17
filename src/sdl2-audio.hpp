#ifndef SDL2_AUDIO_HPP_
#define SDL2_AUDIO_HPP_

#include <SDL2/SDL.h>
#include "console.hpp"

namespace psx {

  class sdl2_audio_t {
    SDL_AudioDeviceID device_id;
    SDL_AudioSpec want;
    SDL_AudioSpec have;

  public:
    sdl2_audio_t();
    ~sdl2_audio_t();

    bool render(psx::output_params_audio_t &params);
  };
}

#endif
