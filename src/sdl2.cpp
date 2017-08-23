#include "sdl2.hpp"


psxact::sdl2::sdl2() {
  SDL_Init(SDL_INIT_VIDEO);

  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

  window = SDL_CreateWindow(
      "psxact",
      SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED,
      640,
      480,
      0);

  renderer = SDL_CreateRenderer(
      window,
      (-1),
      SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  texture = SDL_CreateTexture(
      renderer,
      SDL_PIXELFORMAT_BGR555,
      SDL_TEXTUREACCESS_STREAMING,
      640,
      480);
}

psxact::sdl2::~sdl2() {
  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyTexture(texture);
}

bool psxact::sdl2::render(uint16_t *src_pixels, int w, int h) {
  resize(w, h);

  void *dst_pixels;
  int dst_pitch;
  int src_pitch = 1024 * sizeof(int16_t);

  SDL_LockTexture(texture, nullptr, &dst_pixels, &dst_pitch);

  uint16_t *dst = (uint16_t *)dst_pixels;
  uint16_t *src = (uint16_t *)src_pixels;

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

bool psxact::sdl2::handle_events() {
  SDL_Event event;

  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
      return false;
    }
  }

  return true;
}

void psxact::sdl2::resize(int w, int h) {
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
