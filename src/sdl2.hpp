#ifndef __PSXACT_RENDERER_HPP__
#define __PSXACT_RENDERER_HPP__

#define SDL_MAIN_HANDLED
#include <SDL.h>

namespace psxact {
  struct controller_state {
    bool circle;
    bool cross;
    bool square;
    bool triangle;

    bool dpad_down;
    bool dpad_left;
    bool dpad_right;
    bool dpad_up;

    bool l1;
    bool l2;

    bool r1;
    bool r2;

    bool select;
    bool start;
  };

  static controller_state ctrl;

  class sdl2 {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    int texture_size_x;
    int texture_size_y;

    SDL_GameController *controller;

  public:
    sdl2();

    ~sdl2();

    bool render(uint16_t *src_pixels, int w, int h);

  private:
    bool handle_events();

    void resize(int w, int h);
  };
}

#endif // __PSXACT_RENDERER_HPP__
