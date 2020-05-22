#ifndef SDL2_INPUT_HPP_
#define SDL2_INPUT_HPP_

#include <SDL2/SDL.h>
#include "input/host-device.hpp"

namespace psx {

  class sdl2_input {
  public:
    void from_keyboard(psx::input::host_device &device);
  };
}

#endif
