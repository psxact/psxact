// Copyright 2018 psxact

#include "args.hpp"

#include <getopt.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>


namespace psx {

static const option options[] = {
  { name: "bios", has_arg: required_argument, flag: 0, val: 'b' },
  { name: "game", has_arg: required_argument, flag: 0, val: 'g' },
  { name: "log" , has_arg: required_argument, flag: 0, val: 'l' },
  { 0, 0, 0, 0 }
};


static void show_usage() {
  printf("Usage:\n");
  printf("$ psxact [-b|--bios <file>]\n");
  printf("         [-g|--game <file>]\n");
  printf("         [-l|--log  <counter|cpu|dma|gpu|input|mdec|spu>]\n");
}


static void fail() {
  show_usage();
  exit(1);
}


args_t::args_t(int argc, char **argv) {
  if (argc == 1) {
    fail();
  }

  int index;
  int c;

  this->bios_file_name = "bios.rom";
  this->game_file_name = "";

  while ((c = getopt_long(argc, argv, "b:g:", options, &index)) != -1) {
    switch (c) {
      case 'b': this->bios_file_name = optarg; break;
      case 'g': this->game_file_name = optarg; break;

      default:
        fail();
        break;
    }
  }
}

}  // namespace psx
