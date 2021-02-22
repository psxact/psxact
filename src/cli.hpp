#ifndef SRC_CLI_HPP_
#define SRC_CLI_HPP_

#include "opts.hpp"

std::optional<psx::opts> cli_parse(int argc, char *argv[]);

#endif
