#include <csignal>
#include <cstdio>
#include "args.hpp"
#include "console.hpp"
#include "sdl2-audio.hpp"
#include "sdl2-video.hpp"

void signal_handler(int) {
  exit(0);
}

void run(psx::console_t *console);
void run_headless(psx::console_t *console);

int main(int argc, char *argv[]) {
  signal(SIGINT, signal_handler);

  psx::args_t args(argc, argv);
  psx::console_t *console = new psx::console_t(args);

  if (args.headless) {
    run_headless(console);
  }
  else {
    run(console);
  }

  delete console;

  return 0;
}

void run(psx::console_t *console) {
  psx::sdl2_audio_t audio = psx::sdl2_audio_t();
  psx::sdl2_video_t video = psx::sdl2_video_t();

  do {
    console->run_for_one_frame();

    int16_t *sound;
    int32_t sound_len;

    console->get_audio_params(&sound, &sound_len);
    if (!audio.render(sound, sound_len)) {
      printf("%s\n", SDL_GetError());
      break;
    }

    uint16_t *vram;
    int w;
    int h;

    console->get_video_params(&vram, &w, &h);
    if (!video.render(vram, w, h)) {
      printf("%s\n", SDL_GetError());
      break;
    }
  }
  while (1);
}

void run_headless(psx::console_t *console) {
  do {
    console->run_for_one_frame();
  }
  while (1);
}
