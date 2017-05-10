#include <cassert>
#include "gpu_core.hpp"
#include "../bus.hpp"
#include "../memory/vram.hpp"

uint32_t gpu::data(gpu_state_t *state) {
  if (state->gpu_to_cpu_transfer.run.active) {
    auto lower = vram_transfer(state);
    auto upper = vram_transfer(state);

    return (upper << 16) | lower;
  }

  return 0;
}

uint32_t gpu::stat(gpu_state_t *state) {
  //  19    Vertical Resolution         (0=240, 1=480, when Bit22=1)  ;GP1(08h).2
  //  26    Ready to receive Cmd Word   (0=No, 1=Ready)  ;GP0(...) ;via GP0
  //  27    Ready to send VRAM to CPU   (0=No, 1=Ready)  ;GP0(C0h) ;via GPUREAD
  //  28    Ready to receive DMA Block  (0=No, 1=Ready)  ;GP0(...) ;via GP0

  return (state->status & ~0x00080000) | 0x1c002000;
}

uint32_t gpu::io_read(gpu_state_t *state, int width, uint32_t address) {
  assert(width == bus::BUS_WIDTH_WORD);

  switch (address) {
  case 0x1f801810:
    return data(state);

  case 0x1f801814:
    return stat(state);

  default:
    return 0;
  }
}

void gpu::io_write(gpu_state_t *state, int width, uint32_t address, uint32_t data) {
  assert(width == bus::BUS_WIDTH_WORD);

  switch (address) {
  case 0x1f801810:
    return gp0(state, data);

  case 0x1f801814:
    return gp1(state, data);
  }
}

uint16_t gpu::vram_transfer(gpu_state_t *state) {
  auto &transfer = state->gpu_to_cpu_transfer;
  if (!transfer.run.active) {
    return 0;
  }

  auto data = vram::read(
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

void gpu::vram_transfer(gpu_state_t *state, uint16_t data) {
  auto &transfer = state->cpu_to_gpu_transfer;
  if (!transfer.run.active) {
    return;
  }

  vram::write(
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
