#include "bus.hpp"
#include "cdrom/cdrom_drive.hpp"
#include "cpu/cpu_core.hpp"
#include "gpu/gpu_core.hpp"
#include "renderer.hpp"
#include "timer/timer_core.hpp"

system_state_t *init(const char *bfn, const char *gfn) {
  std::string bios_file_name(bfn);
  std::string game_file_name(gfn);

  auto system = new system_state_t();

  cpu::initialize(&system->cpu_state);
  bus::initialize(system, bios_file_name, game_file_name);

  return system;
}

void run_for_one_frame(system_state_t *system) {
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 33868800 / 60 / 10; j++) {
      cpu::tick(&system->cpu_state);
      timer::tick_timer_2(&system->timer_state);
    }

    cdrom::run(&system->cdrom_state);
  }

  bus::irq(0);
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Usage:\n");
    printf("$ psxact <bios-file-name> <game-file-name>\n");
    return -1;
  }

  auto system = init(argv[1], argv[2]);

  renderer::initialize();

  while (renderer::render()) {
    run_for_one_frame(system);
  }

  renderer::destroy();

  return 0;
}
