// Copyright 2018 psxact

#include "args.hpp"

#include <getopt.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace psx {

static const option options[] = {
  { .name = "bios", .has_arg = required_argument, .flag = 0, .val = 'b' },
  { .name = "game", .has_arg = required_argument, .flag = 0, .val = 'g' },
  { .name = "log",  .has_arg = required_argument, .flag = 0, .val = 'l' },
  { 0, 0, 0, 0 }
};

static void show_usage() {
  printf("Usage:\n");
  printf("$ psxact [-b|--bios <file>]\n");
  printf("         [-g|--game <file>]\n");
  printf("         [-l|--log  <component>]\n");
}

static void fail() {
  show_usage();
  exit(1);
}

args_t::args_t(int argc, char **argv)
  : bios_file_name("bios.rom")
  , game_file_name("")
  , log_cpu(false)
  , log_dma(false)
  , log_gpu(false)
  , log_spu(false)
  , log_mdec(false)
  , log_cdrom(false)
  , log_input(false)
  , log_timer(false) {
  if (argc == 1) {
    fail();
  }

  int index;
  int c;

  while ((c = getopt_long(argc, argv, "b:g:l:", options, &index)) != -1) {
    switch (c) {
      case 'b': this->bios_file_name = optarg; break;
      case 'g': this->game_file_name = optarg; break;
      case 'l':
        if (strncmp(optarg, "cpu", 3) == 0) {
          this->log_cpu = true;
        }
        else if (strncmp(optarg, "dma", 3) == 0) {
          this->log_dma = true;
        }
        else if (strncmp(optarg, "gpu", 3) == 0) {
          this->log_gpu = true;
        }
        else if (strncmp(optarg, "spu", 3) == 0) {
          this->log_spu = true;
        }
        else if (strncmp(optarg, "mdec", 4) == 0) {
          this->log_mdec = true;
        }
        else if (strncmp(optarg, "cdrom", 5) == 0) {
          this->log_cdrom = true;
        }
        else if (strncmp(optarg, "input", 5) == 0) {
          this->log_input = true;
        }
        else if (strncmp(optarg, "timer", 5) == 0) {
          this->log_timer = true;
        }
        break;

      default:
        fail();
        break;
    }
  }
}

}  // namespace psx
