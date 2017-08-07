#include "bus.hpp"
#include "renderer.hpp"

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Usage:\n");
    printf("$ psxact <bios-file-name> <game-file-name>\n");
    return -1;
  }

  bus::init(argv[1], argv[2]);

  renderer::initialize();

  while (renderer::render()) {
    bus::run_for_one_frame();
  }

  renderer::destroy();

  return 0;
}
