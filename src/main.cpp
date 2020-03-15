#include <cstdio>
#include "args.hpp"
#include "console.hpp"
#include "sdl2.hpp"

void run(psx::console_t *console);
void run_headless(psx::console_t *console);

int main(int argc, char *argv[]) {
  psx::args_t args(argc, argv);

  psx::console_t *console = new psx::console_t(args);

  if (args.headless) {
    run_headless(console);
  }
  else {
    run(console);
  }

  return 0;
}

void run(psx::console_t *console) {
  psx::sdl2 renderer;

  uint16_t *vram;
  int w;
  int h;

  do {
    console->run_for_one_frame();
    console->get_video_params(&vram, &w, &h);
  }
  while (renderer.render(vram, w, h));
}

void run_headless(psx::console_t *console) {
  uint16_t *vram;
  int w;
  int h;

  do {
    console->run_for_one_frame();
    console->get_video_params(&vram, &w, &h);
  }
  while (1);
}
