#include <cstdio>
#include "args.hpp"
#include "console.hpp"
#include "sdl2.hpp"


using namespace psx;

int main(int argc, char *argv[]) {
  args_t ctx(argc, argv);

  console_t *console = new console_t(
    ctx.bios_file_name,
    ctx.game_file_name
  );

  sdl2 renderer;

  uint16_t *vram;
  int w;
  int h;

  do {
    console->run_for_one_frame(&vram, &w, &h);
  }
  while (renderer.render(vram, w, h));

  return 0;
}
