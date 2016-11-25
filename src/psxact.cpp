#include "bus.hpp"
#include "cdrom/cdrom_core.hpp"
#include "cpu/cpu_core.hpp"
#include "gpu/gpu_core.hpp"
#include "renderer.hpp"

int main(int argc, char *argv[]) {
  if (argc != 3) {
    return -1;
  }

  freopen("C:/Users/Adam.Becker/stdout.log", "w", stdout);

  std::string bios_file_name(argv[1]);
  std::string game_file_name(argv[2]);

  cpu::initialize();
  bus::initialize(bios_file_name, game_file_name);

  renderer::initialize();

  while (renderer::render()) {
    for (int i = 0; i < 10; i++) {
      cpu::run(33868800 / 60 / 10);
      cdrom::run();
    }

    bus::irq(0);
  }

  renderer::destroy();

  return 0;
}
