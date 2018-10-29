// Copyright 2018 psxact

#include <cstdio>
#include "args.hpp"
#include "console.hpp"
#include "sdl2.hpp"


int main(int argc, char *argv[]) {
  psx::args_t ctx(argc, argv);

  psx::console_t *console = new psx::console_t(
    ctx.bios_file_name,
    ctx.game_file_name);

  psx::sdl2 renderer;

  uint16_t *vram;
  int w;
  int h;

  do {
    console->run_for_one_frame(&vram, &w, &h);
  } while (renderer.render(vram, w, h));

  return 0;
}
