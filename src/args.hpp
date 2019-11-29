// Copyright 2018 psxact

#ifndef ARGS_HPP_
#define ARGS_HPP_

namespace psx {

struct args_t {
  const char *bios_file_name;
  const char *game_file_name;

  args_t(int argc, char **argv);
};

}  // namespace psx

#endif  // ARGS_HPP_
