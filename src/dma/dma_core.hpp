#ifndef PSXACT_DMA_CORE_HPP
#define PSXACT_DMA_CORE_HPP

#include <cstdint>

namespace dma {
  struct state_t {
    uint32_t dpcr = 0x07654321;
    uint32_t dicr = 0x00000000;

    struct {
      uint32_t address;
      uint32_t counter;
      uint32_t control;
    } channels[7];
  };

  uint32_t mmio_read(int size, uint32_t address);

  void mmio_write(int size, uint32_t address, uint32_t data);

  void main();

  void irq_channel(int n);

  void run_channel(int n);
}

#endif //PSXACT_DMA_CORE_HPP
