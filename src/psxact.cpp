#include "bus.hpp"
#include "cpu/cpu_core.hpp"

int main(int argc, char *argv[]) {
  if (argc != 2) {
    return -1;
  }

  std::string bios_file_name(argv[1]);
//std::string game_file_name(argv[2]);

  bus::initialize(bios_file_name);
  cpu::initialize();

  cpu::main();

  return 0;
}
