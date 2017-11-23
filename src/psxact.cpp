#include <cstdio>
#include "console.hpp"
#include "sdl2.hpp"
#include "memory/vram.hpp"


console_t *bus;


int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Usage:\n");
    printf("$ psxact <bios-file-name> <game-file-name>\n");
    return -1;
  }

  bus = new console_t(argv[1], argv[2]);

  sdl2 renderer;

  int x = 0;
  int y = 0;
  int w = 640;
  int h = 480;

  while (1) {
    if (renderer.render(vram::get_pointer(x, y), w, h) == 0) {
      break;
    }

    bus->run_for_one_frame(&x, &y, &w, &h);
  }

  return 0;
}
