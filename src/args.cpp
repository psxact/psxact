#include "args.hpp"

#include <cstdio>
#include <cstdlib>
#include <getopt.h>
#include <unistd.h>


using namespace psx;

static const option options[] = {
  { "bios", required_argument, 0, 'b' },
  { "game", required_argument, 0, 'g' },
  { "log" , required_argument, 0, 'l' },
  { 0 }
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
