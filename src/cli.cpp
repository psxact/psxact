#include "cli.hpp"

#include <cstring>
#include <getopt.h>
#include <optional>

#include "opts-builder.hpp"

using namespace psx;

static const option options[] = {
  { .name = "bios",  .has_arg = required_argument, .flag = 0, .val = 'b' },
  { .name = "game",  .has_arg = required_argument, .flag = 0, .val = 'g' },
  { .name = "log",   .has_arg = required_argument, .flag = 0, .val = 'l' },
  { .name = "patch", .has_arg = required_argument, .flag = 0, .val = 'p' },
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

std::optional<opts> cli_parse(int argc, char **argv) {
  if (argc == 1) {
    fail();
  }

  opts_builder builder;

  int index;
  int c;

  while ((c = getopt_long(argc, argv, "b:g:l:p:", options, &index)) != -1) {
    switch (c) {
      case 'b': builder.put_bios_filename(optarg); break;
      case 'g': builder.put_game_filename(optarg); break;
      case 'l':
        if (strcmp(optarg, "bios") == 0) {
          builder.put_log_enabled(component::bios);
        } else if (strcmp(optarg, "cdrom") == 0) {
          builder.put_log_enabled(component::cdrom);
        } else if (strcmp(optarg, "console") == 0) {
          builder.put_log_enabled(component::console);
        } else if (strcmp(optarg, "cpu") == 0) {
          builder.put_log_enabled(component::cpu);
        } else if (strcmp(optarg, "dcache") == 0) {
          builder.put_log_enabled(component::dcache);
        } else if (strcmp(optarg, "dma") == 0) {
          builder.put_log_enabled(component::dma);
        } else if (strcmp(optarg, "exp1") == 0) {
          builder.put_log_enabled(component::exp1);
        } else if (strcmp(optarg, "exp2") == 0) {
          builder.put_log_enabled(component::exp2);
        } else if (strcmp(optarg, "exp3") == 0) {
          builder.put_log_enabled(component::exp3);
        } else if (strcmp(optarg, "gpu") == 0) {
          builder.put_log_enabled(component::gpu);
        } else if (strcmp(optarg, "input") == 0) {
          builder.put_log_enabled(component::input);
        } else if (strcmp(optarg, "mdec") == 0) {
          builder.put_log_enabled(component::mdec);
        } else if (strcmp(optarg, "memctl") == 0) {
          builder.put_log_enabled(component::memctl);
        } else if (strcmp(optarg, "spu") == 0) {
          builder.put_log_enabled(component::spu);
        } else if (strcmp(optarg, "timer") == 0) {
          builder.put_log_enabled(component::timer);
        } else if (strcmp(optarg, "vram") == 0) {
          builder.put_log_enabled(component::vram);
        } else if (strcmp(optarg, "wram") == 0) {
          builder.put_log_enabled(component::wram);
        } else {
          fprintf(stderr, "unrecognized argument to --log: %s\n", optarg);
        }
        break;

      case 'p':
        if (strcmp(optarg, "skip-intro") == 0) {
          builder.put_patch_enabled(bios_patch::skip_intro);
        } else if (strcmp(optarg, "debug-tty") == 0) {
          builder.put_patch_enabled(bios_patch::debug_tty);
        } else {
          fprintf(stderr, "unrecognized argument to --patch: %s\n", optarg);
        }
        break;

      default:
        fail();
        break;
    }
  }

  return builder.build();
}
