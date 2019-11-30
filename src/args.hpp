// Copyright 2018 psxact

#ifndef ARGS_HPP_
#define ARGS_HPP_

namespace psx {

struct args_t {
  const char *bios_file_name;
  const char *game_file_name;
  bool log_cpu;
  bool log_dma;
  bool log_gpu;
  bool log_spu;
  bool log_mdec;
  bool log_cdrom;
  bool log_input;
  bool log_timer;

  args_t(int argc, char **argv);
};

}  // namespace psx

#endif  // ARGS_HPP_
