#include "gpu_core.hpp"
#include "../bus.hpp"
#include "../memory/vram.hpp"
#include <stdexcept>
#include <cassert>

gpu::state_t gpu::state;

uint32_t gpu::data() {
  if (gpu::state.gpu_to_cpu_transfer.run.active) {
    auto lower = vram_transfer();
    auto upper = vram_transfer();

    return (upper << 16) | lower;
  }

  printf("gpu::read_data()\n");
  return 0;
}

uint32_t gpu::stat() {
  //  19    Vertical Resolution         (0=240, 1=480, when Bit22=1)  ;GP1(08h).2
  //  26    Ready to receive Cmd Word   (0=No, 1=Ready)  ;GP0(...) ;via GP0
  //  27    Ready to send VRAM to CPU   (0=No, 1=Ready)  ;GP0(C0h) ;via GPUREAD
  //  28    Ready to receive DMA Block  (0=No, 1=Ready)  ;GP0(...) ;via GP0

  return (gpu::state.status & ~0x00080000) | 0x1c002000;
}

uint32_t gpu::mmio_read(int size, uint32_t address) {
  assert(size == WORD);

  switch (address) {
    case 0x1f801810: return data();
    case 0x1f801814: return stat();
  }
}

void gpu::mmio_write(int size, uint32_t address, uint32_t data) {
  assert(size == WORD);

  switch (address) {
    case 0x1f801810: return gp0(data);
    case 0x1f801814: return gp1(data);
  }
}

uint16_t gpu::vram_transfer() {
  auto &transfer = gpu::state.gpu_to_cpu_transfer;
  if (!transfer.run.active) {
    return 0;
  }

  auto data = vram::read(transfer.reg.x + transfer.run.x,
                         transfer.reg.y + transfer.run.y);

  transfer.run.x++;

  if (transfer.run.x == transfer.reg.w) {
    transfer.run.x = transfer.reg.x;
    transfer.run.y++;

    if (transfer.run.y == transfer.reg.h) {
      transfer.run.y = 0;
      transfer.run.active = false;
    }
  }

  return data;
}

void gpu::vram_transfer(uint16_t data) {
  auto &transfer = gpu::state.cpu_to_gpu_transfer;
  if (!transfer.run.active) {
    return;
  }

  vram::write(transfer.reg.x + transfer.run.x,
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
