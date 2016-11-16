#ifndef PSXACT_RENDERER_HPP
#define PSXACT_RENDERER_HPP

#include <SDL2/SDL.h>

namespace renderer {
  void destroy();

  void initialize();

  bool render();
}

#endif //PSXACT_RENDERER_HPP
