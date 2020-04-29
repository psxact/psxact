#ifndef SDL2_VIDEO_HPP_
#define SDL2_VIDEO_HPP_

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include "console.hpp"

namespace psx {

  class sdl2_video_t {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    int texture_size_x;
    int texture_size_y;

  public:
    sdl2_video_t();
    ~sdl2_video_t();

    bool render(psx::output_params_video_t &params);

  private:
    bool handle_events();

    void resize(int w, int h);
  };
}  // namespace psx

#endif  // SDL2_VIDEO_HPP_
