#ifndef PSXACT_DMA_CORE_HPP
#define PSXACT_DMA_CORE_HPP

#include <cstdint>

namespace dma {
  struct channel_t {
    uint32_t address;
    uint32_t block_control;
    uint32_t channel_control;
  };

  struct state_t {
    uint32_t dpcr = 0x07654321;
    uint32_t dicr;

    channel_t channels[7];
  };

  void initialize();

  uint32_t mmio_read(int size, uint32_t address);

  void mmio_write(int size, uint32_t address, uint32_t data);

  void main();
}

#endif //PSXACT_DMA_CORE_HPP
