#include "sdl2-video.hpp"

using namespace psx;

static constexpr int window_width = 640;
static constexpr int window_height = 480;

sdl2_video_t::sdl2_video_t()
  : window()
  , renderer()
  , texture()
  , texture_size_x(window_width)
  , texture_size_y(window_height) {

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
}

sdl2_video_t::~sdl2_video_t() {
  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyTexture(texture);
}

bool sdl2_video_t::render(psx::output_params_video_t &params) {
  resize(params.width, params.height);

  void *dst_pixels = nullptr;
  int dst_pitch = 0;
  void *src_pixels = params.buffer;
  int src_pitch = 1024 * sizeof(uint16_t);

  if (SDL_LockTexture(texture, nullptr, &dst_pixels, &dst_pitch) == 0) {
    uint16_t *dst = reinterpret_cast<uint16_t *>(dst_pixels);
    uint16_t *src = reinterpret_cast<uint16_t *>(src_pixels);

    for (int py = 0; py < params.height; py++) {
      for (int px = 0; px < params.width; px++) {
        dst[px] = src[px];
      }

      src += src_pitch / sizeof(uint16_t);
      dst += dst_pitch / sizeof(uint16_t);
    }

    SDL_UnlockTexture(texture);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
  }

  return handle_events();
}

bool sdl2_video_t::handle_events() {
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
    }
  }

  return alive;
}

void sdl2_video_t::resize(int w, int h) {
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
