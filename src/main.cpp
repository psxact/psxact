#include <csignal>
#include <cstdio>
#include <execinfo.h>

#include "cli.hpp"
#include "console.hpp"
#include "opts.hpp"
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

void run(psx::console *console);

int main(int argc, char *argv[]) {
  signal(SIGABRT, signal_handler);
  signal(SIGSEGV, signal_handler);

  auto opt = cli_parse(argc, argv);
  if (!opt.has_value()) {
    psx::util::panic("unable to parse command line arguments");
  } else {
    auto console = new psx::console(opt.value());
    run(console);
    delete console;
  }

  return 0;
}

void run(psx::console *console) {
  SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);

  auto audio = psx::sdl2_audio();
  auto video = psx::sdl2_video();
  auto input = psx::sdl2_input();

  auto i = psx::input_params {};
  auto o = psx::output_params {};

  while (1) {
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
}
