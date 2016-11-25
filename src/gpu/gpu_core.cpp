#include "gpu_core.hpp"
#include "../bus.hpp"
#include "../memory/vram.hpp"
#include <stdexcept>
#include <cassert>

gpu::state_t gpu::state;

static uint32_t read_texture() {
  auto &download = gpu::state.texture_download;
  if (!download.run.remaining) {
    return 0;
  }

  auto data = vram::read(download.run.x, download.run.y);

  download.run.remaining--;
  download.run.x++;

  if (download.run.x == (download.reg.x + download.reg.w)) {
    download.run.x = download.reg.x;
    download.run.y++;
  }

  return data;
}

uint32_t gpu::data() {
  if (gpu::state.texture_download.run.remaining) {
    auto lower = read_texture();
    auto upper = read_texture();

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

  auto result = (gpu::state.status & ~0x00080000) | 0x1c002000;

  printf("gpu::stat(0x%08x)\n", result);

  return result;
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
