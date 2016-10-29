#ifndef PSXACT_DMA_CORE_HPP
#define PSXACT_DMA_CORE_HPP

#include <cstdint>

namespace dma {
  struct state_t {
    uint32_t dpcr = 0x07654321;
    uint32_t dicr;
  };

  void initialize();

  uint32_t mmio_read(int size, uint32_t address);

  void mmio_write(int size, uint32_t address, uint32_t data);
}

#endif //PSXACT_DMA_CORE_HPP
