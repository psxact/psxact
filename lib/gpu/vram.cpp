#include "gpu/core.hpp"

using namespace psx::gpu;

uint32_t core::vram_address(int x, int y) {
  return (2048 * y) + x;
}

uint8_t core::vram_read8(int x, int y) {
  return vram->io_read(address_width::byte, vram_address(x, y));
}

uint16_t core::vram_read(int x, int y) {
  return vram->io_read(address_width::half, vram_address(x * 2, y));
}

void core::vram_write(int x, int y, uint16_t data) {
  vram->io_write(address_width::half, vram_address(x * 2, y), data);
}

uint16_t core::vram_transfer_read() {
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

void core::vram_transfer_write(uint16_t data) {
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
