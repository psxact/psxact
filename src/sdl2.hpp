#ifndef SDL2_HPP_
#define SDL2_HPP_

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

namespace psx {

class sdl2 {
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *texture;
  int texture_size_x;
  int texture_size_y;

 public:
  sdl2();

  ~sdl2();

  bool render(uint16_t *src_pixels, int w, int h);

 private:
  bool handle_events();

  void resize(int w, int h);
};

}  // namespace psx

#endif  // SDL2_HPP_
