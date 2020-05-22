#ifndef SDL2_VIDEO_HPP_
#define SDL2_VIDEO_HPP_

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include "console.hpp"

namespace psx {

  class sdl2_video {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    int texture_size_x;
    int texture_size_y;

  public:
    sdl2_video();
    ~sdl2_video();

    bool render(psx::output_params_video &params);

  private:
    bool handle_events();

    void resize(int w, int h);
  };
}

#endif
