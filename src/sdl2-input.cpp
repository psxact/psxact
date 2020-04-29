#include <SDL2/SDL.h>
#include "sdl2-input.hpp"

using namespace psx;
using namespace psx::input;

void sdl2_input_t::from_keyboard(host_device_t &device) {
  auto keys = SDL_GetKeyboardState(nullptr);

  device.select = host_device_button_t(keys[SDL_SCANCODE_RSHIFT]);
  device.start = host_device_button_t(keys[SDL_SCANCODE_RETURN]);
  device.dpad_up = host_device_button_t(keys[SDL_SCANCODE_UP]);
  device.dpad_right = host_device_button_t(keys[SDL_SCANCODE_RIGHT]);
  device.dpad_down = host_device_button_t(keys[SDL_SCANCODE_DOWN]);
  device.dpad_left = host_device_button_t(keys[SDL_SCANCODE_LEFT]);
  device.left_back_shoulder = host_device_button_t(keys[SDL_SCANCODE_1]);
  device.right_back_shoulder = host_device_button_t(keys[SDL_SCANCODE_3]);
  device.left_front_shoulder = host_device_button_t(keys[SDL_SCANCODE_Q]);
  device.right_front_shoulder = host_device_button_t(keys[SDL_SCANCODE_E]);
  device.button_3 = host_device_button_t(keys[SDL_SCANCODE_W]);
  device.button_2 = host_device_button_t(keys[SDL_SCANCODE_D]);
  device.button_0 = host_device_button_t(keys[SDL_SCANCODE_X]);
  device.button_1 = host_device_button_t(keys[SDL_SCANCODE_A]);
}
