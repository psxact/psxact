#include "dma_core.hpp"
#include "../utility.hpp"

static dma::state_t state;

static inline uint32_t get_channel_index(uint32_t address) {
  return (address >> 4) & 7;
}

static inline uint32_t get_register_index(uint32_t address) {
  return (address >> 2) & 3;
}

void dma::initialize() {
  state.dpcr = 0x07654321;
  state.dicr = 0x00000000;
}

uint32_t dma::mmio_read(int size, uint32_t address){
  auto channel = get_channel_index(address);
  if (channel == 7) {
    switch (get_register_index(address)) {
      case 0: return state.dpcr;
      case 1: return state.dicr;
      case 2: return 0x7ffac68b;
      case 3: return 0x00fffff7;
    }
  }
  else {
    printf("dma::channel_%d::mmio_read(0x%08x)\n", channel, address);
  }

  return 0;
}

void dma::mmio_write(int size, uint32_t address, uint32_t data) {
  auto channel = get_channel_index(address);
  if (channel == 7) {
    switch (get_register_index(address)) {
      case 0: state.dpcr = data & 0xffffffff; printf("DPCR: $%08x\n", state.dpcr); break;
      case 1: state.dicr = data & 0x0fff803f; printf("DICR: $%08x\n", state.dicr); break;
      case 2: break;
      case 3: break;
    }
  }
  else {
    printf("dma::channel_%d::mmio_write(0x%08x, 0x%08x)\n", channel, address, data);
  }
}
