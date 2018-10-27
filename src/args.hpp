#ifndef __psxact_args__
#define __psxact_args__


struct args_t {
  const char *bios_file_name;
  const char *game_file_name;

  args_t(int argc, char **argv);
};


#endif // __psxact_args__
