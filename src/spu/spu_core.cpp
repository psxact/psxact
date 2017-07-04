#include "spu_core.hpp"
#include "../utility.hpp"

uint32_t spu::io_read(spu_state_t *state, int width, uint32_t address) {
  if (utility::log_spu) {
    printf("spu::io_read(%d, 0x%08x)\n", width, address);
  }

  return 0;
}

void spu::io_write(spu_state_t *state, int width, uint32_t address, uint32_t data) {
  if (utility::log_spu) {
    printf("spu::io_write(%d, 0x%08x, 0x%08x)\n", width, address, data);
  }
}
