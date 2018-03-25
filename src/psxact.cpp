#include <cstdio>
#include "console.hpp"
#include "sdl2.hpp"
#include "memory/vram.hpp"


console_t *console;


struct app_context_t {

  const char *bios_file_name = "bios.rom";
  const char *game_file_name = "";
  bool log_counter;
  bool log_cpu;
  bool log_dma;
  bool log_gpu;
  bool log_input;
  bool log_mdec;
  bool log_spu;

};


static void usage() {
  printf("Usage:\n");
  printf("$ psxact [--game <file>]\n");
  printf("         [--bios <file>]\n");
  printf("         [--log-counter]\n");
  printf("         [--log-cpu]\n");
  printf("         [--log-dma]\n");
  printf("         [--log-gpu]\n");
  printf("         [--log-input]\n");
  printf("         [--log-mdec]\n");
  printf("         [--log-spu]\n");
}


static int parse_args(int argc, char *argv[], app_context_t *ctx) {
  if (argc == 1) {
    return 1;
  }

  while (true) {
    argc--;
    argv++;

    if (argc == 0) {
      break;
    }

    if (strcmp(*argv, "--game") == 0) {
      if (argc <= 1) {
        printf("No value specified for `--game'.\n");
        return 1;
      }
      else {
        argc--;
        argv++;
        ctx->game_file_name = *argv;
      }
    }
    else if (strcmp(*argv, "--bios") == 0) {
      if (argc <= 1) {
        printf("No value specified for `--bios'.\n");
        return 1;
      }
      else {
        argc--;
        argv++;
        ctx->bios_file_name = *argv;
      }
    }
    else if (strcmp(*argv, "--log-counter") == 0) {
      ctx->log_counter = 1;
    }
    else if (strcmp(*argv, "--log-cpu") == 0) {
      ctx->log_cpu = 1;
    }
    else if (strcmp(*argv, "--log-dma") == 0) {
      ctx->log_dma = 1;
    }
    else if (strcmp(*argv, "--log-gpu") == 0) {
      ctx->log_gpu = 1;
    }
    else if (strcmp(*argv, "--log-input") == 0) {
      ctx->log_input = 1;
    }
    else if (strcmp(*argv, "--log-mdec") == 0) {
      ctx->log_mdec = 1;
    }
    else if (strcmp(*argv, "--log-spu") == 0) {
      ctx->log_spu = 1;
    }
    else {
      printf("Unknown option: %s\n", *argv);
      return 1;
    }
  }

  return 0;
}


int main(int argc, char *argv[]) {
  app_context_t ctx;

  if (parse_args(argc, argv, &ctx)) {
    usage();
    return 1;
  }

  console = new console_t(
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
