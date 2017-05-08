#ifndef __PSXACT_RENDERER_HPP__
#define __PSXACT_RENDERER_HPP__

#define SDL_MAIN_HANDLED
#include <SDL.h>

namespace renderer {
  void destroy();

  void initialize();

  bool render();
}

#endif // __PSXACT_RENDERER_HPP__
