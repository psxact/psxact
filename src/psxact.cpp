#include <cstdio>
#include "system_core.hpp"
#include "sdl2.hpp"
#include "memory/vram.hpp"

using namespace psxact;

system_core *psxact::system;

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Usage:\n");
    printf("$ psxact <bios-file-name> <game-file-name>\n");
    return -1;
  }

  psxact::system = new system_core(argv[1], argv[2]);

  sdl2 renderer;

  int x = 0;
  int y = 0;
  int w = 640;
  int h = 480;

  while (1) {
    auto fb = vram::get_pointer(x, y);

    if (renderer.render(fb, w, h)) {
      psxact::system->run_for_one_frame(&x, &y, &w, &h);
    }
    else {
      break;
    }
  }

  return 0;
}
