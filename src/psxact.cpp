#include "bus.hpp"
#include "cdrom/cdrom_core.hpp"
#include "cpu/cpu_core.hpp"
#include "gpu/gpu_core.hpp"
#include "renderer.hpp"
#include "timer/timer_core.hpp"

void run_for_one_frame() {
  for (int i = 0; i < 10; i++) {
    for (int i = 0; i < 33868800 / 60 / 10; i++) {
      cpu::tick();
      timer::tick_timer_2();
    }

    cdrom::run();
  }

  bus::irq(0);
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Usage:\n");
    printf("$ psxact <bios-file-name> <game-file-name>\n");
    return -1;
  }

  std::string bios_file_name(argv[1]);
  std::string game_file_name(argv[2]);

  cpu::initialize();
  bus::initialize(bios_file_name, game_file_name);

  renderer::initialize();

  while (renderer::render()) {
    run_for_one_frame();
  }

  renderer::destroy();

  return 0;
}
