#include "bus.hpp"
#include "cpu/cpu_core.hpp"
#include "dma/dma_core.hpp"
#include "gpu/gpu_core.hpp"
#include "renderer.hpp"

int main(int argc, char *argv[]) {
  if (argc != 3) {
    return -1;
  }

  std::string bios_file_name(argv[1]);
  std::string game_file_name(argv[2]);

  cpu::initialize();
  dma::initialize();
  bus::initialize(bios_file_name, game_file_name);

  renderer::initialize();

  while (renderer::render()) {
    cpu::run(33868800 / 60);
  }

  renderer::destroy();

  return 0;
}
