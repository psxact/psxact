#include "gpu/core.hpp"


using namespace psx::gpu;

uint16_t *core_t::vram_data(int x, int y) {
  return (uint16_t *)vram.get_pointer(vram_address(x, y));
}


uint32_t core_t::vram_address(int x, int y) {
  return ((y * 1024) + x) * sizeof(uint16_t);
}


uint16_t core_t::vram_read(int x, int y) {
  return vram.io_read_half(vram_address(x, y));
}


void core_t::vram_write(int x, int y, uint16_t data) {
  vram.io_write_half(vram_address(x, y), data);
}


uint16_t core_t::vram_transfer_read() {
  auto &transfer = gpu_to_cpu_transfer;
  if (!transfer.run.active) {
    return 0;
  }

  uint16_t data = vram_read(
      transfer.reg.x + transfer.run.x,
      transfer.reg.y + transfer.run.y);

  transfer.run.x++;

  if (transfer.run.x == transfer.reg.w) {
    transfer.run.x = 0;
    transfer.run.y++;

    if (transfer.run.y == transfer.reg.h) {
      transfer.run.y = 0;
      transfer.run.active = false;
    }
  }

  return data;
}


void core_t::vram_transfer_write(uint16_t data) {
  auto &transfer = cpu_to_gpu_transfer;
  if (!transfer.run.active) {
    return;
  }

  vram_write(
      transfer.reg.x + transfer.run.x,
      transfer.reg.y + transfer.run.y, uint16_t(data));

  transfer.run.x++;

  if (transfer.run.x == transfer.reg.w) {
    transfer.run.x = 0;
    transfer.run.y++;

    if (transfer.run.y == transfer.reg.h) {
      transfer.run.y = 0;
      transfer.run.active = false;
    }
  }
}
