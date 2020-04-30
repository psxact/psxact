#ifndef ARGS_HPP_
#define ARGS_HPP_

namespace psx::args {
  extern const char *bios_file_name;
  extern const char *game_file_name;
  extern bool gamma_correction;
  extern bool headless;
  extern bool log_cpu;
  extern bool log_dma;
  extern bool log_gpu;
  extern bool log_spu;
  extern bool log_mdec;
  extern bool log_cdrom;
  extern bool log_input;
  extern bool log_timer;

  void init(int argc, char **argv);
}  // namespace psx::args

#endif  // ARGS_HPP_
