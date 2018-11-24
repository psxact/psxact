// Copyright 2018 psxact

#include "sdl2.hpp"

#include <cstdio>

namespace psx {

static constexpr int window_width = 640;
static constexpr int window_height = 480;

sdl2::sdl2() {
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);

  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

  window = SDL_CreateWindow(
    "psxact",
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    window_width,
    window_height,
    0);

  renderer = SDL_CreateRenderer(
    window,
    (-1),
    SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  texture = SDL_CreateTexture(
    renderer,
    SDL_PIXELFORMAT_BGR555,
    SDL_TEXTUREACCESS_STREAMING,
    window_width,
    window_height);

  //
  // Game Controller
  //

  controller = nullptr;

  for (int i = 0; i < SDL_NumJoysticks(); i++) {
    if (SDL_IsGameController(i)) {
      controller = SDL_GameControllerOpen(i);

      if (controller) {
        break;
      }
    }
  }

  if (controller == nullptr) {
    printf("[SDL2] No controller connected.\n");
  }
}

sdl2::~sdl2() {
  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyTexture(texture);
}

bool sdl2::render(uint16_t *src_pixels, int w, int h) {
  resize(w, h);

  void *dst_pixels;
  int dst_pitch;
  int src_pitch = 1024 * sizeof(uint16_t);

  SDL_LockTexture(texture, nullptr, &dst_pixels, &dst_pitch);

  uint16_t *dst = reinterpret_cast<uint16_t *>(dst_pixels);
  uint16_t *src = reinterpret_cast<uint16_t *>(src_pixels);

  for (int py = 0; py < h; py++) {
    for (int px = 0; px < w; px++) {
      dst[px] = src[px];
    }

    src += src_pitch / sizeof(uint16_t);
    dst += dst_pitch / sizeof(uint16_t);
  }

  SDL_UnlockTexture(texture);
  SDL_RenderCopy(renderer, texture, nullptr, nullptr);
  SDL_RenderPresent(renderer);

  return handle_events();
}

static void controller_button(controller_state_t *ctrl, uint8_t button, bool isPressed) {
  switch (button) {
    case SDL_CONTROLLER_BUTTON_DPAD_UP:
      ctrl->dpad_up = isPressed;
      break;

    case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
      ctrl->dpad_down = isPressed;
      break;

    case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
      ctrl->dpad_left = isPressed;
      break;

    case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
      ctrl->dpad_right = isPressed;
      break;

    case SDL_CONTROLLER_BUTTON_A:
      ctrl->cross = isPressed;
      break;

    case SDL_CONTROLLER_BUTTON_B:
      ctrl->circle = isPressed;
      break;

    case SDL_CONTROLLER_BUTTON_X:
      ctrl->square = isPressed;
      break;

    case SDL_CONTROLLER_BUTTON_Y:
      ctrl->triangle = isPressed;
      break;

    case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
      ctrl->l1 = isPressed;
      break;

    case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
      ctrl->r1 = isPressed;
      break;

    case SDL_CONTROLLER_BUTTON_BACK:
      ctrl->select = isPressed;
      break;

    case SDL_CONTROLLER_BUTTON_START:
      ctrl->start = isPressed;
      break;
  }
}

static void controller_axis(controller_state_t *ctrl, uint8_t axis, int value) {
  switch (axis) {
    case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
      ctrl->l2 = value > 16383;
      break;

    case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
      ctrl->r2 = value > 16383;
      break;
  }
}

bool sdl2::handle_events() {
  SDL_Event event;

  bool alive = true;

  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT:
        alive = false;
        break;

      case SDL_KEYDOWN:
        alive = event.key.keysym.sym != SDLK_ESCAPE;
        break;

      case SDL_CONTROLLERBUTTONDOWN:
        controller_button(&ctrl, event.cbutton.button, 0);
        break;

      case SDL_CONTROLLERBUTTONUP:
        controller_button(&ctrl, event.cbutton.button, 1);
        break;

      case SDL_CONTROLLERAXISMOTION:
        controller_axis(&ctrl, event.caxis.axis, event.caxis.value);
        break;
    }
  }

  return alive;
}

void sdl2::resize(int w, int h) {
  if (texture_size_x == w && texture_size_y == h) {
    return;
  }

  texture_size_x = w;
  texture_size_y = h;

  SDL_DestroyTexture(texture);

  texture = SDL_CreateTexture(
    renderer,
    SDL_PIXELFORMAT_BGR555,
    SDL_TEXTUREACCESS_STREAMING,
    w,
    h);
}

}  // namespace psx
