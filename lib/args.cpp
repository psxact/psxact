#include "args.hpp"

#include <getopt.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace psx;

static const option options[] = {
  { .name = "bios",             .has_arg = required_argument, .flag = 0, .val = 'b' },
  { .name = "game",             .has_arg = required_argument, .flag = 0, .val = 'g' },
  { .name = "log",              .has_arg = required_argument, .flag = 0, .val = 'l' },
  { .name = "patch",            .has_arg = required_argument, .flag = 0, .val = 'p' },
  { .name = "headless",         .has_arg =       no_argument, .flag = 0, .val = 'h' },
  { .name = "gamma-correction", .has_arg =       no_argument, .flag = 0, .val = 'y' },
  {},
};

static void show_usage() {
  printf("Usage:\n");
  printf("$ psxact [-b|--bios <file>]\n");
  printf("         [-g|--game <file>]\n");
  printf("         [-l|--log <component>]\n");
  printf("         [-p|--patch <skip-intro|debug-tty>]\n");
  printf("         [--headless]\n");
}

static void fail() {
  show_usage();
  exit(1);
}

const char *args::bios_file_name = "bios.rom";
const char *args::game_file_name = "";
bool args::gamma_correction = false;
bool args::headless = false;
bool args::log_cpu = false;
bool args::log_dma = false;
bool args::log_gpu = false;
bool args::log_spu = false;
bool args::log_mdec = false;
bool args::log_cdrom = false;
bool args::log_input = false;
bool args::log_timer = false;
bool args::bios_patch_debug_tty = false;
bool args::bios_patch_skip_intro = false;

void args::init(int argc, char **argv) {
  if (argc == 1) {
    fail();
  }

  int index;
  int c;

  while ((c = getopt_long(argc, argv, "b:g:l:p:", options, &index)) != -1) {
    switch (c) {
      case 'b': args::bios_file_name = optarg; break;
      case 'g': args::game_file_name = optarg; break;
      case 'h': args::headless = true; break;
      case 'y': args::gamma_correction = true; break;
      case 'l':
        if (strcmp(optarg, "cpu") == 0) {
          args::log_cpu = true;
        } else if (strcmp(optarg, "dma") == 0) {
          args::log_dma = true;
        } else if (strcmp(optarg, "gpu") == 0) {
          args::log_gpu = true;
        } else if (strcmp(optarg, "spu") == 0) {
          args::log_spu = true;
        } else if (strcmp(optarg, "mdec") == 0) {
          args::log_mdec = true;
        } else if (strcmp(optarg, "cdrom") == 0) {
          args::log_cdrom = true;
        } else if (strcmp(optarg, "input") == 0) {
          args::log_input = true;
        } else if (strcmp(optarg, "timer") == 0) {
          args::log_timer = true;
        } else {
          fprintf(stderr, "unrecognized argument to --log: %s\n", optarg);
        }
        break;

      case 'p':
        if (strcmp(optarg, "skip-intro") == 0) {
          bios_patch_skip_intro = true;
        } else if (strcmp(optarg, "debug-tty") == 0) {
          bios_patch_debug_tty = true;
        } else {
          fprintf(stderr, "unrecognized argument to --patch: %s\n", optarg);
        }
        break;

      default:
        fail();
        break;
    }
  }
}
