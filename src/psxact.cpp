#include <cstdio>
#include "system_core.hpp"
#include "renderer.hpp"

psxact::system_core *psxact::system;

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Usage:\n");
    printf("$ psxact <bios-file-name> <game-file-name>\n");
    return -1;
  }

  psxact::system = new psxact::system_core(argv[1], argv[2]);

  renderer::initialize();

  while (renderer::render()) {
    psxact::system->run_for_one_frame();
  }

  renderer::destroy();

  return 0;
}
