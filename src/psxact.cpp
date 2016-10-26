#include "bus.hpp"
#include "cpu/cpu_core.hpp"

int main(int argc, char *argv[]) {
  if (argc != 2) {
    return -1;
  }

  std::string file_name(argv[1]);

  bus::initialize(file_name);

  cpu::core_t core = cpu::core_t();

  core.initialize();
  core.main();

  return 0;
}
