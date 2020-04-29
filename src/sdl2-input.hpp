#ifndef SDL2_INPUT_HPP_
#define SDL2_INPUT_HPP_

#include <SDL2/SDL.h>
#include "input/host-device.hpp"

namespace psx {

  class sdl2_input_t {
  public:
    void from_keyboard(psx::input::host_device_t &device);
  };
}  // namespace psx

#endif  // SDL2_INPUT_HPP_
