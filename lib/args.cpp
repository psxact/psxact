#include "args.hpp"

#include <getopt.h>
#include <unistd.h>
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>

#include "util/panic.hpp"

using namespace psx;
using namespace psx::util;

static const option options[] = {
  { .name = "bios",             .has_arg = required_argument, .flag = 0, .val = 'b' },
  { .name = "game",             .has_arg = required_argument, .flag = 0, .val = 'g' },
  { .name = "log",              .has_arg = required_argument, .flag = 0, .val = 'l' },
  { .name = "patch",            .has_arg = required_argument, .flag = 0, .val = 'p' },
  {},
};

static FILE *bios_file = nullptr;
static FILE *game_file = nullptr;
static game_type game_file_type = game_type::none;
static uint32_t log_flags = 0;
static uint32_t bios_patch_flags = 0;

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

void args::init(int argc, char **argv) {
  if (argc == 1) {
    fail();
  }

  int index;
  int c;

  while ((c = getopt_long(argc, argv, "b:g:l:p:", options, &index)) != -1) {
    switch (c) {
      case 'b': put_bios_filename(optarg); break;
      case 'g': put_game_filename(optarg); break;
      case 'l':
        if (strcmp(optarg, "cpu") == 0) {
          put_log_enabled(component::cpu);
        } else if (strcmp(optarg, "dma") == 0) {
          put_log_enabled(component::dma);
        } else if (strcmp(optarg, "gpu") == 0) {
          put_log_enabled(component::gpu);
        } else if (strcmp(optarg, "spu") == 0) {
          put_log_enabled(component::spu);
        } else if (strcmp(optarg, "mdec") == 0) {
          put_log_enabled(component::mdec);
        } else if (strcmp(optarg, "cdrom") == 0) {
          put_log_enabled(component::cdrom);
        } else if (strcmp(optarg, "input") == 0) {
          put_log_enabled(component::input);
        } else if (strcmp(optarg, "timer") == 0) {
          put_log_enabled(component::timer);
        } else {
          fprintf(stderr, "unrecognized argument to --log: %s\n", optarg);
        }
        break;

      case 'p':
        if (strcmp(optarg, "skip-intro") == 0) {
          put_patch_enabled(bios_patch::skip_intro);
        } else if (strcmp(optarg, "debug-tty") == 0) {
          put_patch_enabled(bios_patch::debug_tty);
        } else {
          fprintf(stderr, "unrecognized argument to --patch: %s\n", optarg);
        }
        break;

      default:
        fail();
        break;
    }
  }

  if (bios_file == nullptr) {
    panic("Required argument '--bios|-b' not specified.");
  }
}

void args::put_bios_filename(const char *val) {
  bios_file = fopen(val, "rb");

  if (bios_file == nullptr) {
    panic("Unable to open '%s' for reading.", val);
  }
}

void args::put_game_filename(const char *val) {
  game_file = fopen(val, "rb");

  if (game_file == nullptr) {
    panic("Unable to open '%s' for reading.", val);
  }

  if (strstr(val, ".exe") || strstr(val, ".psexe")) {
    game_file_type = game_type::psexe;
  } else {
    game_file_type = game_type::disc;
  }
}

void args::put_log_enabled(component val) {
  log_flags |= 1 << int(val);
}

void args::put_patch_enabled(bios_patch val) {
  bios_patch_flags |= 1 << int(val);
}

FILE *args::get_bios_file() {
  return bios_file;
}

std::optional<FILE *> args::get_game_file() {
  if (get_game_file_type() == game_type::none) {
    return std::nullopt;
  } else {
    return game_file;
  }
}

game_type args::get_game_file_type() {
  return game_file_type;
}

bool args::get_log_enabled(component val) {
  return log_flags & (1 << int(val));
}

bool args::get_patch_enabled(bios_patch val) {
  return bios_patch_flags & (1 << int(val));
}
