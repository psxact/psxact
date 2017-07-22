#include "bus.hpp"
#include "cdrom/cdrom_drive.hpp"
#include "cpu/cpu_core.hpp"
#include "gpu/gpu_core.hpp"
#include "renderer.hpp"
#include "timer/timer_core.hpp"

system_state_t *initialize(const char *bfn, const char *gfn) {
  system_state_t *system = new system_state_t();

  cdrom::initialize(&system->cdrom_state, gfn);
  cpu::initialize(&system->cpu_state);
  bus::initialize(system, bfn);

  return system;
}

void run_for_one_frame(system_state_t *system) {
  const int ITERATIONS = 2;

  const int CPU_FREQ = 33868800;
  const int CYCLES_PER_FRAME = CPU_FREQ / 60 / ITERATIONS;

  for (int i = 0; i < CYCLES_PER_FRAME; i++) {
    cpu::tick(&system->cpu_state);

    for (int j = 0; j < ITERATIONS; j++) {
      timer::tick(&system->timer_state);
      cdrom::tick(&system->cdrom_state);
    }
  }

  bus::irq(0);
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Usage:\n");
    printf("$ psxact <bios-file-name> <game-file-name>\n");
    return -1;
  }

  system_state_t *system = initialize(argv[1], argv[2]);

  renderer::initialize();

  while (renderer::render()) {
    run_for_one_frame(system);
  }

  renderer::destroy();

  return 0;
}
