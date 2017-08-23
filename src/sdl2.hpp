#ifndef __PSXACT_RENDERER_HPP__
#define __PSXACT_RENDERER_HPP__

#define SDL_MAIN_HANDLED
#include <SDL.h>

namespace psxact {
  class sdl2 {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    int texture_size_x;
    int texture_size_y;

  public:
    sdl2();

    ~sdl2();

    bool render(uint16_t *fb, int x, int y, int w, int h);

  private:
    bool handle_events();

    void resize(int w, int h);
  };
}

#endif // __PSXACT_RENDERER_HPP__
