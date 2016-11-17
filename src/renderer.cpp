#include "renderer.hpp"
#include "gpu/gpu_core.hpp"

SDL_Window *window;
SDL_Surface *surface;
SDL_Event event;

void renderer::destroy() {
  SDL_DestroyWindow(window);
}

void renderer::initialize() {
  SDL_Init(SDL_INIT_VIDEO);

  window = SDL_CreateWindow("psxact",
                            SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED, 1024, 512, 0);

  surface = SDL_GetWindowSurface(window);
}

uint32_t color_16_to_24(uint32_t color) {
  auto r = ((color << 3) & 0xf8);
  auto g = ((color >> 2) & 0xf8);
  auto b = ((color >> 7) & 0xf8);

  return (0xff << 24) | (r << 16) | (g << 8) | (b << 0);
}

bool renderer::render() {
  SDL_LockSurface(surface);

  auto pixels = (uint32_t *)surface->pixels;

  for (uint32_t i = 0; i < 1024 * 512; i++) {
    *pixels++ = color_16_to_24( gpu::vram.h[i] );
  }

  SDL_UnlockSurface(surface);
  SDL_UpdateWindowSurface(window);

  return !SDL_PollEvent(&event) || event.type != SDL_QUIT;
}
