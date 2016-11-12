#include "bus.hpp"
#include "cpu/cpu_core.hpp"
#include "dma/dma_core.hpp"
#include "gpu/gpu_core.hpp"

#include <SDL2/SDL.h>

uint32_t color_16_to_24(uint32_t color) {
  auto r = ((color << 3) & 0xf8);
  auto g = ((color >> 2) & 0xf8);
  auto b = ((color >> 7) & 0xf8);

  return (0xff << 24) | (r << 16) | (g << 8) | (b << 0);
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    return -1;
  }

  std::string bios_file_name(argv[1]);
  std::string game_file_name(argv[2]);

  cpu::initialize();
  dma::initialize();
  bus::initialize(bios_file_name, game_file_name);

  SDL_Init(SDL_INIT_VIDEO);

  auto window = SDL_CreateWindow("psxact",
                                 SDL_WINDOWPOS_CENTERED,
                                 SDL_WINDOWPOS_CENTERED, 1024, 512, 0);

  auto surface = SDL_GetWindowSurface(window);

  SDL_Event event;

  while (true) {
    cpu::run(33868800 / 60);

    // put the frame buffer on the screen
    //

    SDL_LockSurface(surface);

    auto pixels = (uint32_t *)surface->pixels;

    for (uint32_t i = 0; i < 1024 * 512; i++) {
      *pixels++ = color_16_to_24( gpu::vram.h[i] );
    }

    SDL_UnlockSurface(surface);
    SDL_UpdateWindowSurface(window);

    if (SDL_PollEvent(&event) && event.type == SDL_QUIT) {
      break;
    }
  }

  SDL_DestroyWindow(window);

  return 0;
}
