#ifndef PSXACT_RENDERER_HPP
#define PSXACT_RENDERER_HPP

#define SDL_MAIN_HANDLED
#include <SDL.h>

namespace renderer {
  void destroy();

  void initialize();

  bool render();
}

#endif //PSXACT_RENDERER_HPP
