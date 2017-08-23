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

bool psxact::sdl2::render(uint16_t *fb, int x, int y, int w, int h) {
  resize(w, h);

  void *texture_pixels;
  int pitch;

  SDL_LockTexture(texture, nullptr, &texture_pixels, &pitch);

  uint16_t *pixels = (uint16_t *)texture_pixels;
  uint16_t *colors = fb;

  colors += (y * 1024) + x;

  for (int py = 0; py < h; py++) {
    for (int px = 0; px < w; px++) {
      pixels[0] = colors[px];
      pixels++;
    }

    colors += 1024;
    // pixels += 640;
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

  printf("resizing texture to %d x %d\n", w, h);

  SDL_DestroyTexture(texture);

  texture = SDL_CreateTexture(
      renderer,
      SDL_PIXELFORMAT_BGR555,
      SDL_TEXTUREACCESS_STREAMING,
      w,
      h);
}
