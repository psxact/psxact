#include <csignal>
#include <cstdio>
#include <execinfo.h>
#include "args.hpp"
#include "console.hpp"
#include "sdl2-audio.hpp"
#include "sdl2-video.hpp"
#include "sdl2-input.hpp"

void signal_handler(int sig) {
  void *array[10];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 10);

  // print out all the frames to stderr
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, 2);
  exit(1);
}

void run(psx::console_t *console);
void run_headless(psx::console_t *console);

int main(int argc, char *argv[]) {
  signal(SIGABRT, signal_handler);
  signal(SIGSEGV, signal_handler);

  psx::args::init(argc, argv);
  psx::console_t *console = new psx::console_t();

  if (psx::args::headless) {
    run_headless(console);
  } else {
    run(console);
  }

  delete console;

  return 0;
}

void run(psx::console_t *console) {
  SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);

  psx::sdl2_audio_t audio = psx::sdl2_audio_t();
  psx::sdl2_video_t video = psx::sdl2_video_t();
  psx::sdl2_input_t input = psx::sdl2_input_t();

  psx::input_params_t i = {};
  psx::output_params_t o = {};

  do {
    input.from_keyboard(i.device1);

    console->run_for_one_frame(i, o);

    if (!audio.render(o.audio)) {
      printf("%s\n", SDL_GetError());
      break;
    }

    if (!video.render(o.video)) {
      printf("%s\n", SDL_GetError());
      break;
    }
  }
  while (1);
}

void run_headless(psx::console_t *console) {
  psx::input_params_t i = {};
  psx::output_params_t o = {};

  do {
    console->run_for_one_frame(i, o);
  }
  while (1);
}
